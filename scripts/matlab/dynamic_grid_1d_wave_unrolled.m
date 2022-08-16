clear, clc, close all;

Fs = 44100;
duration = 5;
nF = ceil(Fs*duration);

doDisplacementPlot = true;
plotRate = 5;
plotStart = 1;% floor(nF*.9);
plotYLim = 2.5e-4;

%{
    0 = displacement
    1 = velocity
%}
outputMode = 1;

doDisplacementCorrection = true;

outPos = linspace(3/7, 3/7, nF);

k = 1/Fs;
kSq = k^2;
% Variable tension, density, radius -- thus variable wavespeed.
L = 1.05;
% T = [linspace(100, 5000, floor(nF/7)), linspace(5000, 100, ceil(nF - nF/7))];
T = [linspace(1000, 5000, floor(nF/7)), ...
    linspace(5000, 100, ceil(nF/7)), ...
    linspace(100, 5000, floor(nF/7)), ...
    linspace(5000, 100, ceil(nF/7)), ...
    linspace(100, 5000, floor(nF/7)), ...
    linspace(5000, 100, ceil(nF/7)), ...
    linspace(100, 5000, floor(nF/7)) ...
];
rho = 1000*ones(1, nF);
r = 9e-4*ones(1, nF);

hVec = zeros(nF, 1);
cVec = zeros(nF, 1);
NFracVec = zeros(nF, 1);
NVec = zeros(nF, 1);
tVec = (1:nF)/Fs;

if doDisplacementCorrection
    FVec = zeros(nF, 1);
    
    % Prevents division by zero, but may not be necessary after all because of
    % how the equations rearrange
    epsilon = 0;
    % Damping coefficient for the spring force
    sigc = 1;

    sigck = sigc/k;
    sigckp = 1 + sigck;
    sigckm = 1 - sigck;
    
    restoringForce = sigckm / sigckp;
end

n = 1;
A = pi*r(n)^2;
c = sqrt(T(n)/(rho(n)*A));
cSq = c^2;

t60 = [18.5, 10];
[sig0, sig1] = t60ToSigma(c, 0, 100, t60(1), 1000, t60(2));

h = sqrt(cSq*kSq + 4*sig1*k);
hSq = h^2;
NFrac = L/h;
N = floor(NFrac);
NPrev = N;

% Split state in two.
wWidth = 1;
Mu = N - wWidth;
% Just one grid interval for the right-hand system.
Mw = wWidth;
% State matrices
u = zeros(3, Mu + 1);
w = zeros(3, Mw + 1);

% Output buffer
out = [0, 0, 0];
% Output vector
y = zeros(nF, 1);

% Excitation stuff -------------------------------------------------------------
excitationDuration = 5; % samples
excitationCentre = .25; % normalised string location
excitationWidth = 5; % gridpoints (raised cosine)

ex = zeros(excitationDuration + 1, excitationWidth);
inPos = floor(N*excitationCentre - excitationWidth/2);
excitationWidth = min(N-2, excitationWidth);
force = 5e-6;

temporalDist = 1 - cos(pi*(0:excitationDuration)/(excitationDuration));
spatialDist = force*(1 - cos(2*pi*(0:excitationWidth)/(excitationWidth)));
ex(:,inPos:inPos + excitationWidth) = temporalDist'.*spatialDist;
mesh(ex), title('Excitation shape');
% ex = [ex; zeros(duration*Fs - excitationDuration, size(ex, 2))];
clear spatialDist temporalDist;
% ------------------------------------------------------------------------------

tic
for n=1:nF
    next = mod(n, 3) + 1;
    indices = wrapIndices([next - 1, next - 2], 3);
    curr = indices(1);
    prev = indices(2);
    
    % Calculate current wavespeed
    c = sqrt(T(n)/(rho(n)*pi*r(n)^2));
    cSq = c^2;
    cVec(n) = c;
    
    [sig0, sig1] = t60ToSigma(c, 0, 100, t60(1), 1000, t60(2));
    
    % Calculate current grid spacing
    h = sqrt(cSq*kSq + 4*sig1*k);
    % Record stability; should always be 1. IS ALWAYS 1. STUPID.
    hVec(n) = h/sqrt(cSq*kSq + 4*sig1*k);
    hSq = h^2;
    % From that, calculate current fractional number of gridpoints
    NFrac = L/h;
    NFracVec(n) = NFrac;
    % Get nearest integer number of gridpoints less than the above
    N = floor(NFrac);
    NVec(n) = N;
    % Get the difference
    alpha = NFrac - N;
    
    if N ~= NPrev
        assert(abs(N-NPrev) == 1);
        
        if N > NPrev
            % Create cubic interpolator 
            cubicIp = [alpha * (alpha + 1) / -((alpha + 2) * (alpha + 3)); ...
                        2 * alpha / (alpha + 2); ...
                        2 / (alpha + 2); ...
                        2 * alpha / -((alpha + 3) * (alpha + 2))]';
            % Add a spatial gridpoint
            u = [u [0; 0; 0]];
            u(curr, end) = cubicIp(1:3) * [u(curr, end-1:end), w(curr, 1)]';
            u(prev, end) = cubicIp(1:3) * [u(prev, end-1:end), w(prev, 1)]';
        elseif N < NPrev
            % Remove a spatial gridpoint
            u = u(:, 1:end-1);
        end
        
        Mu = N - wWidth;
        Mw = wWidth;    
    end
    
    lambdaSq = cSq*kSq/hSq;
    S = 2*sig1*k/hSq;
    
    coeffs = [ ...
        2*(1 - lambdaSq - S), ...
        lambdaSq + S, ...
        sig0*k + 2*S - 1, ...
        -S
    ] / (1 + sig0*k);
    
    % Excitation (assume always applied to left system)
    if n <= size(ex, 1)
        u(curr, 1:size(ex, 2)) = u(curr, 1:size(ex, 2)) + ex(n, :);
    end
    
    % Virtual gridpoints
    % Quadratic interpolator
    ip = [-(alpha - 1) / (alpha + 1), 1, (alpha - 1) / (alpha + 1)];
    
    % virtualGridPoints = [u_{M+1}^n; w_{-1}^n]
    % (Since this uses freq-dependent decay, I think there should be previous
    % virtual gridpoints too -- u_{l-1}^{n-1}, u_{l+1}^{n-1}. I guess the 1D 
    % case is stable enough to work without those.)     
    virtualGridpoints = [ip(3) * u(curr, end) + ip(2) * w(curr, 1) + 0; ...
        ip(1) * u(curr, end-1) + ip(2) * u(curr, end) + ip(3) * w(curr, 1)]; 
    
    virtualGridpointsPrev = [ip(3) * u(prev, end) + ip(2) * w(prev, 1) + 0; ...
        ip(1) * u(prev, end-1) + ip(2) * u(prev, end) + ip(3) * w(prev, 1)]; 
    
    for l=2:Mu
        u(next, l) = coeffs * [...
            u(curr, l); ...
            u(curr, l + 1) + u(curr, l - 1); ...
            u(prev, l); ...
            u(prev, l + 1) + u(prev, l - 1) ...
        ];
    end
    
    l = Mu+1;
    u(next, l) = coeffs * [...
        u(curr, l); ...
        virtualGridpoints(1) + u(curr, l - 1); ...
        u(prev, l); ...
        virtualGridpointsPrev(1) + u(prev, l - 1) ...
    ];

    l = 1;
    w(next, l) = coeffs * [...
        w(curr, l); ...
        w(curr, l + 1) + virtualGridpoints(2); ...
        w(prev, l); ...
        w(prev, l + 1) + virtualGridpointsPrev(2) ...
    ];
    
    if doDisplacementCorrection
        % Displacement correction -- mitigate discontinuities when wavespeed
        % increases
        onemalphaSigckp = (1 - alpha)*sigckp;
        % Calculate correction force
        etaNext = w(next, 1) - u(next, end);
        etaPrev = w(prev, 1) - u(prev, end);

        a = (h*onemalphaSigckp) / ...
            (2*(h*(alpha + epsilon) + kSq*onemalphaSigckp));
        
        F = a * (restoringForce * etaPrev + etaNext);

        % The plot of the correcting force is pretty cool.
        FVec(n) = F;

        % Multiply F by the scheme leftovers.
        correctionF = (F*kSq) / (h*(1 + sig0*k));       
        
        % Add the force to u; subtract it from w.
        u(next, end) = u(next, end) + correctionF;
        w(next, 1) = w(next, 1) - correctionF;
    end
    
    % Get output
    outLoc = min(max(3, NFrac*outPos(n)), NFrac - 3);
    outSamp = interpolateCubic(u(next, :), outLoc);

    if outputMode == 0
        y(n) = outSamp;
    else
        out(1) = outSamp;  
        y(n) = (out(1) - out(3)) / (2*k);
        out(3) = out(2);
        out(2) = out(1);
    end
    
    NPrev = N;
    
    if doDisplacementPlot && n > plotStart && mod(n, plotRate) == 0
        % Grid point locations
        hLocsLeft = (0:(size(u, 2) - 1))*h;
        hLocsRight = (fliplr(L - (0:(size(w, 2) - 1))*h));
        
        % Plot left system (with left outer boundary)
        plot(hLocsLeft, u(next, :), 'LineWidth' , 1.5, 'Color', 'k');
        ax = gca;
        ax.FontSize = 12;
        xlim([0, L]);
        xlabel('Spatial domain (m)', 'FontSize', 16);
        ylabel('Displacement (m)', 'FontSize', 16);
        grid on;
        hold on;
        % Plot right system (with right outer boundary)
        plot(hLocsRight, w(next, :), 'Linewidth', 1.5, 'Color', 'r');
        
        scatter(outLoc*h, outSamp, 64, 'filled');
        text(outLoc*h, plotYLim/8, "$y[n]$", 'interpreter', 'latex', 'FontSize', 16);
        
%         title('Displacement');
        ylim([-plotYLim, plotYLim]);
        
        hold off;
        drawnow;
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

switch outputMode
    case 0
        y = y*1e3;
end

soundsc(y, Fs);
tfPlot(y, Fs);

fontSize = 16;
latexFontSize = 20;

figure;
plot(tVec, cVec, 'LineWidth', 2);
xlabel('Time (s)', 'FontSize', fontSize);
ylabel('$c$', 'interpreter', 'latex', 'FontSize', latexFontSize);
% title('Wavespeed', 'FontSize', fontSize);
grid on;

figure;
plot(tVec, NFracVec, tVec, NVec, 'LineWidth', 2);
xlabel('Time (s)', 'FontSize', fontSize);
ylabel('$N$', 'interpreter', 'latex', 'FontSize', latexFontSize);
% title('Size of spatial domain', 'FontSize', fontSize);
grid on;

figure;
spectrogram(y, 512, 64, 512, Fs, 'yaxis');