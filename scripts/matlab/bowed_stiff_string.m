clear, close all;

doDisplacementPlot = false;
plotRateLimit = 25;
plotYLim = 5e-4;

Fs = 44100;
duration = 5;
nF = ceil(duration*Fs);
timesteps = linspace(0, duration, duration*Fs);
% 'displacement' or 'velocity'
outType = 'velocity';

% Bowing force and velocity vectors. Time/param pairs
fbdata = [
    0, 0;
    0.9, 1;
    1.5, 1;
    2.25, 1;
    2.5, 1;
    3.5, 0;
    duration, 0
];

vbdata = [
    0, 0;
    0.8, 1;
    1.5, 1;
    2.0, -1;
    3.5, -1
    4, 0
    duration, 0
];
      
outPos = linspace(1/13, 3/7, Fs*duration);
inPos = linspace(.1, .2, Fs*duration);

% Preparation parameters
doPreparation = true;
preparationPos = linspace(.9, .1, Fs*duration);
% Angular frequencies for the SHO and cubic nonlinear oscillator components of
% the preparation.
omegaC = [16, .16];
% Convert from frequency to angular frequency.
omegaC = omegaC*2*pi;
omegaC = [100, 10];
% Damping coefficient for the preparation.
% Seems to blow up for sigmaP < -.1 & sigmaP > 3
sigmaP = 100;
% NB: higher values of sigmaP (~.1) and lower values of omegaC ([1, 1]) for a
% harmonic; lower values of sigmaP (~.001) and higher values of omegaC 
% ([100, 1000]) for a stopped note.

% String params
k = 1/Fs;
% Length (m)
L = 1;
% Density (kg/m^3)
rho = 7850;
% Cross-sectional radius (m)
% r = 1e-4;
r = 5e-4;
% Cross-sectional area (m^2)
A = pi*r^2;
% Tension (N)
% T = 15;
T = 100;
% Wavespeed (m/s)
% c = 200;
c = sqrt(T/(rho*A));
% Young's modulus (Pa)
E = 2e11;
% Area moment of inertia (m^4)
I = 0.25*pi*r^4;
% Stiffness (m^2/s)
% kappa = 9;
kappa = sqrt((E*I)/(rho*A*L^4));

% Loss
T60_0 = 13.5;
s0 = (6*log(10))/T60_0;
% Freq-dependent loss
T60_1 = 3000;
s1 = (6*log(10))/T60_1;
% s1 = 0.005;
omega0 = 100;
omega1 = 1000;
T60_0 = 12.2;
T60_1 = 1.3;
[s0, s1] = t60ToSigma(c, kappa, omega0, T60_0, omega1, T60_1);

% Physical grid-spacing
h_ = c^2*k^2 + 4*s1*k;
h__ = sqrt(h_^2 + 16*kappa^2*k^2);
h = sqrt((h_+h__)/ 2);
clear h_ h__;

% Number of physical grid points, which must be an integer...
N = floor(L/h);
% Recalculate h based on forced integer
h = L/N;

% Calculate string coefficients
rhoA = rho*A;
kSq = k^2;
lambda = (k*c)/h;
lambdaSq = lambda^2;
mu = (kappa*k)/(h^2);
nu = (2*s1*k)/(h^2);

lambdaSqPlus4MuSq = lambdaSq + 4*mu^2

rawStringCoeffs = [
    2 - 2*lambdaSq - 6*mu^2 - 2*nu, ...
    2 - 2*lambdaSq - 5*mu^2 - 2*nu, ...
    s0*k - 1 + 2*nu, ...
    -(mu^2), ...
    lambdaSq + 4*mu^2 + nu, ...
    -nu, ...
];
schemeDivisor = 1 + s0*k;
% Divide all coefficients by the divisor to the FDS.
stringCoeffs = rawStringCoeffs/schemeDivisor;

% Excitation: bow
bowPos = N*inPos;
bowCoeffs = [ ...
    2*(1/k + s0), ...
    2*(c^2/h^2 - 1/k^2 + 3*kappa^2/h^4 + 2*s1/k*h^2), ...
    2*(1/k^2 - 2*s1/k*h^2), ...
    -(c^2/h^2 + 4*kappa^2/h^4 + 2*s1/k*h^2), ...
    2*s1/k*h^2, ...
    kappa^2/h^4 ...
];
% Friction model free parameter
a = 100;
% Part of Phi(vRel)... leaves vRel * exp(-a*vRel^2) to be calculated
phi1 = sqrt(2*a) * exp(.5);
% Bits of the scheme expansion that are applied to the excitation
excitationCoefficient = (k^2 * phi1) / (rho*A*(1 + s0*k));
% The part of Newton-Raphson that isn't dependent on vRel
nr1 = (2/k) + (2*s0);
% Bow force
fB = .2 * interp1(fbdata(:,1), fbdata(:,2), timesteps);
% Bow velocity
vB = -.2 * interp1(vbdata(:,1), vbdata(:,2), timesteps);
% Newton-Raphson stuff
maxIterations = 100;
tol = 1e-6;

% Bits of the scheme expansion that are applied to the preparation
preparationCoeffs = [ ...
    (omegaC(1)^2)/2, ...
    (omegaC(2)^4)/2, ...
    sigmaP/k
];
% preparationForceCoefficient = k^2 / (rho*A*(1 + s0*k));
preparationForceCoefficient = k^2 / (1 + s0*k);
% preparation position
colPos = N*preparationPos;

% Set up a circular buffer of buffers to store the next, current and previous
% states of the system.
u = zeros(3, N+1);
out = [0, 0, 0];

% Output vector.
y = zeros(ceil(duration*Fs), 1);

% for plotting preparation force
Fvec = zeros(ceil(duration*Fs), 1);

% Simply-supported boundary condition, so iterate from 3:N-1 and handle 2 and N
% separately.
lRange = 3:N-1;

if doDisplacementPlot
    figure('Position', [100, 100, 1500, 800])
end

tic
for n=1:nF
    next = mod(n, 3) + 1;
    indices = wrapIndices([next - 1, next - 2], 3);
    curr = indices(1);
    prev = indices(2);
    
    l = 2;
    u(next, l) = stringCoeffs(2)*u(curr, l) ...
        + stringCoeffs(3)*u(prev, l) ...
        + stringCoeffs(4)*u(curr, l+2) ...
        + stringCoeffs(5)*(u(curr, l+1) + u(curr, l-1)) ...
        + stringCoeffs(6)*(u(prev, l+1) + u(prev, l-1));
    
    for l=lRange
        u(next, l) = stringCoeffs(1)*u(curr, l) ...
            + stringCoeffs(3)*u(prev, l) ...
            + stringCoeffs(4)*(u(curr, l+2) + u(curr, l-2)) ...
            + stringCoeffs(5)*(u(curr, l+1) + u(curr, l-1)) ...
            + stringCoeffs(6)*(u(prev, l+1) + u(prev, l-1));
    end
    
    l = N;
    u(next, l) = stringCoeffs(2)*u(curr, l) ...
        + stringCoeffs(3)*u(prev, l) ...
        + stringCoeffs(4)*u(curr, l-2) ...
        + stringCoeffs(5)*(u(curr, l+1) + u(curr, l-1)) ...
        + stringCoeffs(6)*(u(prev, l+1) + u(prev, l-1));
    
    % Compute bow
    pBow = min(N-3, max(5, bowPos(n)));
    uB = [ ...
        interpolateCubic(u(curr, :), pBow - 2), ...
        interpolateCubic(u(curr, :), pBow - 1), ...
        interpolateCubic(u(curr, :), pBow), ...
        interpolateCubic(u(curr, :), pBow + 1), ...
        interpolateCubic(u(curr, :), pBow + 2)
    ];
    uBPrev = [ ...
        interpolateCubic(u(prev, :), pBow - 1), ...
        interpolateCubic(u(prev, :), pBow), ...
        interpolateCubic(u(prev, :), pBow + 1)
    ];
    b = bowCoeffs(1)*vB(n) + ...
        bowCoeffs(2)*uB(3) + ...
        bowCoeffs(3)*uBPrev(2) + ...
        bowCoeffs(4)*(uB(2) + uB(4)) + ...
        bowCoeffs(5)*(uBPrev(1) + uBPrev(3)) + ...
        bowCoeffs(6)*(uB(1) + uB(5));
    vRel = 0;
    vRelPrev = 0;
    
    % Newton-Raphson
    for i = 1:maxIterations
        nr2 = phi1 * fB(n) * exp(-a*(vRelPrev^2));

        vRel = vRelPrev - ...
            (nr1*vRelPrev + nr2*vRelPrev + b) / ...
            (nr1 + nr2*(1 - 2*a*vRelPrev^2));
        % threshold check
        if abs(vRel - vRelPrev) < tol
            break;
        end
        vRelPrev = vRel;
    end
%     i
    
    excitation = excitationCoefficient * fB(n) * vRel * exp(-a * vRel^2);
    
    u(next, :) = extrapolateCubic(u(next, :), pBow, h, -excitation);
    
    if doPreparation
        % Calculate preparation position
        pCol = min(N-3, max(5, colPos(n)));
        
        eta = [ ...
            interpolateCubic(u(curr, :), pCol - 2), ...
            interpolateCubic(u(curr, :), pCol - 1), ...
            interpolateCubic(u(curr, :), pCol), ...
            interpolateCubic(u(curr, :), pCol + 1), ...
            interpolateCubic(u(curr, :), pCol + 2)
        ];
        etaPrev = [ ...
            interpolateCubic(u(prev, :), pCol - 1), ...
            interpolateCubic(u(prev, :), pCol), ...
            interpolateCubic(u(prev, :), pCol + 1)
        ];

        prepFa = kSq*(preparationCoeffs(1) + preparationCoeffs(2)*eta(3)^2 + preparationCoeffs(3));
        prepFb = kSq*(preparationCoeffs(1) + preparationCoeffs(2)*eta(3)^2 - preparationCoeffs(3));

        etaNext = (...
            rawStringCoeffs(1)*eta(3) + ... % eta_p^n
            (rawStringCoeffs(3) - prepFb)*etaPrev(2) + ... % eta_p^{n-1}
            rawStringCoeffs(4)*(eta(1) + eta(5)) + ... % eta_{p±2}^n
            rawStringCoeffs(5)*(eta(2) + eta(4)) + ... % eta_{p±1}^n
            rawStringCoeffs(6)*(etaPrev(1) + etaPrev(3)) ... % eta_{p±1}^{n-1}
        ) / (schemeDivisor + prepFa);

        F = -preparationCoeffs(1)*(etaNext+etaPrev(2)) ...
            -preparationCoeffs(2)*(eta(3)^2)*(etaNext+etaPrev(2)) ...
            -preparationCoeffs(3)*(etaNext-etaPrev(2));
        Fvec(n) = F;

        u(next, :) = extrapolateCubic(u(next, :), pCol, h, F*preparationForceCoefficient);
    end
    
    % Output read position
    outIdx = min(max(3, N*outPos(n)), N-2);
    outSamp = interpolateCubic(u(curr, :), outIdx);
    
    switch outType
        case 'velocity'
            out(1) = outSamp;
            
            y(n) = (out(1) - out(3)) / (2*k);
            
            out(3) = out(2);
            out(2) = out(1);
        otherwise
            y(n) = outSamp;
    end
    
    if doDisplacementPlot && mod(n, plotRateLimit) == 0
        plot(u(curr, :), 'LineWidth', 1.5), ...
            title('String displacement'), ...
            ylim([-plotYLim, plotYLim]);
        hold on
        plot(pBow, u(curr, round(pBow)), 'rd', 'LineWidth', 2, 'MarkerSize', 10);
        plot(pCol, u(curr, round(pCol)), 'ko', 'LineWidth', 2, 'MarkerSize', 10);
        hold off
        drawnow;% limitrate;
    end
    
    pctDone = 1000*n/nF;
    prevPctDone = 1000*(n-1)/nF;
    if mod(floor(pctDone), 10) == 0 && mod(floor(prevPctDone), 10) == 9
        if exist('nbytes', 'var')
            fprintf(repmat('\b', 1, nbytes));
        end
        nbytes = fprintf("%d%%", floor(pctDone*.1));
    end
end
toc

clear indices next curr prev excitation outSamp;

switch outType
    case 'displacement'
        y = y*1e3;
end

% Play it and plot it.
soundsc(y, Fs);
figure('Position', [100, 400, 500, 500]),...
    subplot(211), plot(fB), title('Bow Force'), ...
    subplot(212), plot(vB), title('Bow Velocity');
tfPlot(y, Fs, [450, 50, 1000, 300]);
figure('Position', [600, 400, 500, 500]), ...
    plot(Fvec), title('preparation force');
figure('Position', [1100, 400, 500, 500]), ...
    spectrogram(y, 512, 64, 512, Fs, 'yaxis', 'MinThreshold', -90);

%%
% close all;
% fontSize = 24;
% a = 50;
% vRel = linspace(-1, 1, 2000);
% plot(vRel, sqrt(2*a).*vRel.*exp(-a*vRel.^2 + 1/2), 'k', 'LineWidth', 2),...
%     title("$\Phi(v_{rel}), a = " + num2str(a) + "$", 'interpreter', 'latex', 'FontSize', fontSize), ...
%     xlabel("$v_{rel}$", 'interpreter', 'latex', 'FontSize', fontSize), ...
%     ylabel("$\Phi(v_{rel})$", 'interpreter', 'latex', 'FontSize', fontSize), ...
%     grid on, ...
%     ylim([-1.1, 1.1]);
