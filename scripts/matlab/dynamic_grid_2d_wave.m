clear; close all; clc;

Fs = 44100;
duration = 2;
nF = ceil(duration*Fs);

doDisplacementPlot = false;
plotRate = 1;
plotStart = 1;
% plotStart = nF * .9;
% plotStart = 19770;
plotZLim = 1;
%{
    0 = plot3
    1 = pcolor
%}
plotType = 0;

%{
    0 = displacement
    1 = velocity
%}
outputMode = 1;

doZ = true;
doDisplacementCorrection = true;

% Output read position
outPosl = linspace(.05, .8, nF);
outPosm = linspace(.95, .1, nF);

k = 1/Fs;
kSq = k^2;

% Arbitrary dimensions (m)
Lx = .95; % For reasons unknown, meshgrid seems to swap its dimensions when Lx and Ly are equal
Ly = 1;

% Material properties: tension (N/m), density (kg/m^3), thickness (m)
T = linspace(1000, 10000, nF) + 250*sin(2*pi*3.5*linspace(0, duration, nF));
rho = [linspace(100, 2000, floor(nF/2)), linspace(2000, 300, ceil(nF/2))];
H = [linspace(5e-6, 2.5e-5, floor(nF/2)), linspace(2.5e-5, 3e-6, ceil(nF/2))];
cScalar = 1;

if doDisplacementCorrection
    FVec = zeros(nF, 1);
    
    % 0 < eps << 1, prevents division by zero... but isn't necessary after all 
    % because of how the connection force equation rearranges.
    epsilon = 0;
    % Damping coefficient for the spring force
    sigc = 1;

    sigck = sigc/k;
    sigckp = 1 + sigck;
    sigckm = 1 - sigck;
    
    restoringForce = sigckm / sigckp;
end

% Some vectors for plotting/debugging
hVec = zeros(nF, 1);
cVec = zeros(nF, 1);
NxFracVec = zeros(nF, 1);
NxVec = zeros(nF, 1);
NyFracVec = zeros(nF, 1);
NyVec = zeros(nF, 1);
tVec = (1:nF)/Fs;

n = 1;
c = cScalar * sqrt(T(n)/(rho(n)*H(n)));
cSq = c^2;

% Loss
t60 = [30, 20];
[sig0, sig1] = t60ToSigma(c, 0, 100, t60(1), 10000, t60(2));

% Spatial grid spacing and number of spatial gridpoints
h = sqrt(2*(cSq*kSq + 4*sig1*k));
hSq = h^2;
NxFrac = Lx/h;
Nx = floor(NxFrac);
NyFrac = Ly/h;
Ny = floor(NyFrac);
NxPrev = Nx;
NyPrev = Ny;

% Split state into chunks.
% u is the main chunk, v lies at the end of the x axis; w lies at the end of the
% y axis; z makes up the remainder.
% Width of v and w matrices
vWidth = 1;
wWidth = 1;
Mxu = Nx - vWidth;
Myu = Ny - wWidth;

% State matrices
u = zeros(Mxu + 1, Myu + 1, 3);
v = zeros(vWidth + 1, Myu + 1, 3);
w = zeros(Mxu + 1, wWidth + 1, 3);
z = zeros(vWidth + 1, wWidth + 1, 3);

% Excitation... 2D raised cosine
force = 1;
inCentreX = NxFrac*.8;
inCentreY = NyFrac*.2;
inSize = floor(NxFrac*.2);
inStartX = floor(inCentreX - inSize*.5);
inStartY = floor(inCentreY - inSize*.5);
cos2D = force*.5*(1-cos(pi*(0:inSize)/(inSize/2)));
cos3D = cos2D'*cos2D;
u(inStartX:inStartX + inSize, inStartY:inStartY + inSize, 1) = cos3D;
mesh(u(:,:,1));
clear cos2D cos3D;

% Output buffer
out = [0, 0, 0];
% Output vector
y = zeros(nF, 1);

if doDisplacementPlot
    stateFig = figure;
end

tic
for n=1:duration*Fs
    next = mod(n, 3) + 1;
    indices = wrapIndices([next - 1, next - 2], 3);
    curr = indices(1);
    prev = indices(2);
    
    c = cScalar * sqrt(T(n)/(rho(n)*H(n)));
    cVec(n) = c;
    cSq = c^2;
    
    [sig0, sig1] = t60ToSigma(c, 0, 100, t60(1), 10000, t60(2));

    % Spatial grid spacing and number of spatial gridpoints
    h = sqrt(2*(cSq*kSq + 4*sig1*k));
    hSq = h^2;
    
    % Calculate current fractional number of gridpoints
    NxFrac = Lx/h;
    NxFracVec(n) = NxFrac;
    NyFrac = Ly/h;
    NyFracVec(n) = NyFrac;
    % Get nearest integer number of gridpoints less than the above
    Nx = floor(NxFrac);
    NxVec(n) = Nx;
    Ny = floor(NyFrac);
    NyVec(n) = Ny;
    % Get the difference
    alphax = NxFrac - Nx;
    alphay = NyFrac - Ny;
    
    %% Check for changes to the dimensions of the grid.
    if Nx ~= NxPrev
        if Nx > NxPrev
            % Create interpolator 
            cubicIp = [alphax * (alphax + 1) / -((alphax + 2) * (alphax + 3)); ...
                2 * alphax / (alphax + 2); ...
                2 / (alphax + 2); ...
                2 * alphax / -((alphax + 3) * (alphax + 2))]';
            
            % Add a spatial gridpoint
            u(end+1, :, 1) = 0;
            w(end+1, :, 1) = 0;

            u(end, :, curr) = cubicIp * [u(end-1:end, :, curr); v(1:2, :, curr)];
            u(end, :, prev) = cubicIp * [u(end-1:end, :, prev); v(1:2, :, prev)];
            
            if doZ
                w(end, :, curr) = cubicIp * [w(end-1:end, :, curr); z(1:2, :, curr)];
                w(end, :, prev) = cubicIp * [w(end-1:end, :, prev); z(1:2, :, prev)];
            end
        elseif Nx < NxPrev
            % Remove a spatial gridpoint
            u = u(1:end-1, :, :);
            w = w(1:end-1, :, :);
        end
        
        Mxu = Nx - vWidth; 
    end
    
    if Ny ~= NyPrev
        if Ny > NyPrev
            cubicIp = [alphay * (alphay + 1) / -((alphay + 2) * (alphay + 3)); ...
                2 * alphay / (alphay + 2); ...
                2 / (alphay + 2); ...
                2 * alphay / -((alphay + 3) * (alphay + 2))]';
            
            u(:, end+1, 1) = 0;
            v(:, end+1, 1) = 0;
            
            u(:, end, curr) = cubicIp * [u(:, end-1:end, curr), w(:, 1:2, curr)]';
            u(:, end, prev) = cubicIp * [u(:, end-1:end, prev), w(:, 1:2, prev)]';
            
            if doZ
                v(:, end, curr) = cubicIp * [v(:, end-1:end, curr), z(:, 1:2, curr)]';
                v(:, end, prev) = cubicIp * [v(:, end-1:end, prev), z(:, 1:2, prev)]';
            end
        elseif Nx < NyPrev
            u = u(:, 1:end-1, :);
            v = v(:, 1:end-1, :);
        end
        
        Myu = Ny - wWidth;
    end
    
    % Compute coefficients
    lambda = (k*c)/h;
    lambdaSq = lambda^2;

    S = (2*sig1*k)/(hSq);

    coeffs = [
        2*(1 - 2*(lambdaSq + S)), ...
        sig0*k - 1 + 4*S, ...
        lambdaSq + S, ...
        -S ...
    ] / (1 + sig0*k);

    %% Virtual gridpoints    
    % Quadratic interpolators
    % Don't need 2D interpolators because dimensions are always aligned between
    % subsystems.
    ipx = [-(alphax - 1) / (alphax + 1), 1, (alphax - 1) / (alphax + 1)];
    ipy = [-(alphay - 1) / (alphay + 1), 1, (alphay - 1) / (alphay + 1)];
   
    % First row to be applied to right extremity of u
    % Second row to left extremity of v
    virtualGridPointsUVPrev = [ipx(3) * u(end, :, prev) + ipx(2) * v(1, :, prev) + ipx(1) * v(2, :, prev); ...
        ipx(1) * u(end-1, :, prev) + ipx(2) * u(end, :, prev) + ipx(3) * v(1, :, prev)];
    virtualGridPointsUV = [ipx(3) * u(end, :, curr) + ipx(2) * v(1, :, curr) + ipx(1) * v(2, :, curr); ...
        ipx(1) * u(end-1, :, curr) + ipx(2) * u(end, :, curr) + ipx(3) * v(1, :, curr)];

    % First column to be apprevplied to top extremity of u
    % Second column to bottom extremity of w
    virtualGridPointsUWPrev = [ipy(3) * u(:, end, prev) + ipy(2) * w(:, 1, prev) + ipy(1) * w(:, 2, prev), ...
        ipy(1) * u(:, end-1, prev) + ipy(2) * u(:, end, prev) + ipy(3) * w(:, 1, prev)];
    virtualGridPointsUW = [ipy(3) * u(:, end, curr) + ipy(2) * w(:, 1, curr) + ipy(1) * w(:, 2, curr), ...
        ipy(1) * u(:, end-1, curr) + ipy(2) * u(:, end, curr) + ipy(3) * w(:, 1, curr)];

    % First row to be applied to right extremity of w
    % Second row to left extremity of z
    virtualGridPointsWZPrev = [ipx(3) * w(end, :, prev) + ipx(2) * z(1, :, prev) + ipx(1) * z(2, :, prev); ...
        ipx(1) * w(end-1, :, prev) + ipx(2) * w(end, :, prev) + ipx(3) * z(1, :, prev)];
    virtualGridPointsWZ = [ipx(3) * w(end, :, curr) + ipx(2) * z(1, :, curr) + ipx(1) * z(2, :, curr); ...
        ipx(1) * w(end-1, :, curr) + ipx(2) * w(end, :, curr) + ipx(3) * z(1, :, curr)];

    % First column to be applied to top extremity of v
    % Second column to bottom extremity of z
    virtualGridPointsVZPrev = [ipy(3) * v(:, end, prev) + ipy(2) * z(:, 1, prev) + ipy(1) * z(:, 2, prev), ...
        ipy(1) * v(:, end-1, prev) + ipy(2) * v(:, end, prev) + ipy(3) * z(:, 1, prev)];
    virtualGridPointsVZ = [ipy(3) * v(:, end, curr) + ipy(2) * z(:, 1, curr) + ipy(1) * z(:, 2, curr), ...
        ipy(1) * v(:, end-1, curr) + ipy(2) * v(:, end, curr) + ipy(3) * z(:, 1, curr)];

    %% Scheme update
    lRange = 2:Mxu;
    mRange = 2:Myu;
    u(lRange, mRange, next) = coeffs(1)*u(lRange, mRange, curr) + ...
        coeffs(2)*u(lRange, mRange, prev) + ...
        coeffs(3)*( ...
            u(lRange + 1, mRange, curr) + ...
            u(lRange - 1, mRange, curr) + ...
            u(lRange, mRange + 1, curr) + ...
            u(lRange, mRange - 1, curr) ...
        ) + ...
        coeffs(4)*(...
            u(lRange + 1, mRange, prev) + ...
            u(lRange - 1, mRange, prev) + ...
            u(lRange, mRange + 1, prev) + ...
            u(lRange, mRange - 1, prev) ...
        );
    
    % u, top edge, interaction with w
    lRange = 2:Mxu;
    mRange = Myu + 1;
    u(lRange, mRange, next) = coeffs(1)*u(lRange, mRange, curr) + ...
        coeffs(2)*u(lRange, mRange, prev) + ...
        coeffs(3)*( ...
            u(lRange + 1, mRange, curr) + ...
            u(lRange - 1, mRange, curr) + ...
            virtualGridPointsUW(lRange, 1) + ...
            u(lRange, mRange - 1, curr) ...
        ) + ...
        coeffs(4)*(...
            u(lRange + 1, mRange, prev) + ...
            u(lRange - 1, mRange, prev) + ...
            virtualGridPointsUWPrev(lRange, 1) + ...
            u(lRange, mRange - 1, prev) ...
        );
    
    % u, right edge, interaction with v
    lRange = Mxu + 1;
    mRange = 2:Myu;
    u(lRange, mRange, next) = coeffs(1)*u(lRange, mRange, curr) + ...
        coeffs(2)*u(lRange, mRange, prev) + ...
        coeffs(3)*( ...
            virtualGridPointsUV(1, mRange) + ...
            u(lRange - 1, mRange, curr) + ...
            u(lRange, mRange + 1, curr) + ...
            u(lRange, mRange - 1, curr) ...
        ) + ...
        coeffs(4)*(...
            virtualGridPointsUVPrev(1, mRange) + ...
            u(lRange - 1, mRange, prev) + ...
            u(lRange, mRange + 1, prev) + ...
            u(lRange, mRange - 1, prev) ...
        );
    
    % v, left edge, interaction with u
    lRange = 1;
    mRange = 2:Myu;
    v(lRange, mRange, next) = coeffs(1)*v(lRange, mRange, curr) + ...
        coeffs(2)*v(lRange, mRange, prev) + ...
        coeffs(3)*( ...
            v(lRange + 1, mRange, curr) + ...
            virtualGridPointsUV(2, mRange) + ...
            v(lRange, mRange + 1, curr) + ...
            v(lRange, mRange - 1, curr) ...
        ) + ...
        coeffs(4)*(...
            v(lRange + 1, mRange, prev) + ...
            virtualGridPointsUVPrev(2, mRange) + ...
            v(lRange, mRange + 1, prev) + ...
            v(lRange, mRange - 1, prev) ...
        );
    
    % w, bottom edge, interaction with u
    lRange = 2:Mxu;
    mRange = 1;
    w(lRange, mRange, next) = coeffs(1)*w(lRange, mRange, curr) + ...
        coeffs(2)*w(lRange, mRange, prev) + ...
        coeffs(3)*( ...
            w(lRange + 1, mRange, curr) + ...
            w(lRange - 1, mRange, curr) + ...
            w(lRange, mRange + 1, curr) + ...
            virtualGridPointsUW(lRange, 2) ...
        ) + ...
        coeffs(4)*(...
            w(lRange + 1, mRange, prev) + ...
            w(lRange - 1, mRange, prev) + ...
            w(lRange, mRange + 1, prev) + ...
            virtualGridPointsUWPrev(lRange, 2) ...
        );
    
    if doZ
        % u, top corner, intersection with v and w
        lRange = Mxu + 1;
        mRange = Myu + 1;
        u(lRange, mRange, next) = coeffs(1)*u(lRange, mRange, curr) + ...
            coeffs(2)*u(lRange, mRange, prev) + ...
            coeffs(3)*( ...
                virtualGridPointsUV(1, mRange) + ... % u(lRange + 1, mRange, curr) + ...
                u(lRange - 1, mRange, curr) + ...
                virtualGridPointsUW(lRange, 1) + ... % u(lRange, mRange + 1, curr) + ...
                u(lRange, mRange - 1, curr) ...
            ) + ...
            coeffs(4)*(...
                virtualGridPointsUVPrev(1, mRange) + ... % u(lRange + 1, mRange, prev) + ...
                u(lRange - 1, mRange, prev) + ...
                virtualGridPointsUWPrev(lRange, 1) + ... % u(lRange, mRange + 1, prev) + ...
                u(lRange, mRange - 1, prev) ...
            );
        
        % v, intersection with u and z
        lRange = 1;
        mRange = Myu + 1;
        v(lRange, mRange, next) = coeffs(1)*v(lRange, mRange, curr) + ...
            coeffs(2)*v(lRange, mRange, prev) + ...
            coeffs(3)*( ...
                v(lRange + 1, mRange, curr) + ...
                virtualGridPointsUV(2, mRange) + ... %v(lRange - 1, mRange, curr) + ...
                virtualGridPointsVZ(lRange, 1) + ... %v(lRange, mRange + 1, curr) + ...
                v(lRange, mRange - 1, curr) ...
            ) + ...
            coeffs(4)*(...
                v(lRange + 1, mRange, prev) + ...
                virtualGridPointsUVPrev(2, mRange) + ... %v(lRange - 1, mRange, prev) + ...
                virtualGridPointsVZPrev(lRange, 1) + ... %v(lRange, mRange + 1, prev) + ...
                v(lRange, mRange - 1, prev) ...
            );
    
        % w, intersection with u and z
        lRange = Mxu + 1;
        mRange = 1;
        w(lRange, mRange, next) = coeffs(1)*w(lRange, mRange, curr) + ...
            coeffs(2)*w(lRange, mRange, prev) + ...
            coeffs(3)*( ...
                virtualGridPointsWZ(1, mRange) + ... % w(lRange + 1, mRange, curr) + ...
                w(lRange - 1, mRange, curr) + ...
                w(lRange, mRange + 1, curr) + ...
                virtualGridPointsUW(lRange, 2) ... % ...
            ) + ...
            coeffs(4)*(...
                virtualGridPointsWZPrev(1, mRange) + ... % w(lRange + 1, mRange, prev) + ...
                w(lRange - 1, mRange, prev) + ...
                w(lRange, mRange + 1, prev) + ...
                virtualGridPointsUWPrev(lRange, 2) ... % w(lRange, mRange - 1, prev) ...
            );
    
        % z, intersection with v and w
        lRange = 1;
        mRange = 1;
        z(lRange, mRange, next) = coeffs(1)*z(lRange, mRange, curr) + ...
            coeffs(2)*z(lRange, mRange, prev) + ...
            coeffs(3)*( ...
                z(lRange + 1, mRange, curr) + ...
                virtualGridPointsWZ(2, 1) + ... % z(lRange - 1, mRange, curr) + ...
                z(lRange, mRange + 1, curr) + ...
                virtualGridPointsVZ(1, 2) ... % z(lRange, mRange - 1, curr) ...
            ) + ...
            coeffs(4)*(...
                z(lRange + 1, mRange, prev) + ...
                virtualGridPointsWZPrev(2, 1) + ... % z(lRange - 1, mRange, prev) + ...
                z(lRange, mRange + 1, prev) + ...
                virtualGridPointsVZPrev(1, 2) ...% z(lRange, mRange - 1, prev) ...
            );
    end
    
    %% Displacement correction
    if doDisplacementCorrection
        onemalphax = 1 - alphax;
        onemalphay = 1 - alphay;
        
        ax = (h*onemalphax*sigckp) / ...
            (2*(h*(alphax + epsilon) + kSq*onemalphax*sigckp));
        ay = (h*onemalphay*sigckp) / ...
            (2*(h*(alphay + epsilon) + kSq*onemalphay*sigckp));
        
        % Calculate correction force
        etaUVNext = v(1, :, next) - u(end, :, next);
        etaUVPrev = v(1, :, prev) - u(end, :, prev);
        etaUWNext = w(:, 1, next) - u(:, end, next);
        etaUWPrev = w(:, 1, prev) - u(:, end, prev);
        etaVWNext = w(end, 1, next) - v(1, end, next);
        etaVWPrev = w(end, 1, prev) - v(1, end, prev);
        
        Fuv = ax * (restoringForce * etaUVPrev + etaUVNext);
        Fuw = ay * (restoringForce * etaUWPrev + etaUWNext);
        Fvw = sqrt(ax^2 + ay^2) * (restoringForce * etaVWPrev + etaVWPrev);

        % Multiply F by the scheme leftovers.
        correctionF = kSq / (h*(1 + sig0*k));
        correctionFuv = Fuv*correctionF;
        correctionFuw = Fuw*correctionF;
        correctionFvw = Fvw*correctionF;
        
        if doZ
            etaUZNext = z(1, 1, next) - u(end, end, next);
            etaUZPrev = z(1, 1, prev) - u(end, end, prev);
            etaVZNext = z(1, 1, next) - v(1, end, next);
            etaVZPrev = z(1, 1, prev) - v(1, end, prev);
            etaWZNext = z(1, 1, next) - w(end, 1, next);
            etaWZPrev = z(1, 1, prev) - w(end, 1, prev);
            
            Fuz = sqrt(ax^2 + ay^2) * (restoringForce * etaUZPrev + etaUZNext);
            Fvz = ay * (restoringForce * etaVZPrev + etaVZNext);
            Fwz = ax * (restoringForce * etaWZPrev + etaWZNext);
            
            correctionFuz = Fuz*correctionF;
            correctionFvz = Fvz*correctionF;
            correctionFwz = Fwz*correctionF;
            
            u(end, 1:end-1, next) = u(end, 1:end-1, next) + correctionFuv(1:end-1);
            u(1:end-1, end, next) = u(1:end-1, end, next) + correctionFuw(1:end-1);
            u(end, end, next) = u(end, end, next) ...
                + .5*(correctionFuv(end) + correctionFuw(end)) ...
                + correctionFuz;
            
            v(1, 1:end-1, next) = v(1, 1:end-1, next) - correctionFuv(1:end-1);
            v(1, end, next) = v(1, end, next) ...
                - .5*(correctionFvz + correctionFuv(end)) ...
                - correctionFvw;
            
            w(1:end-1, 1, next) = w(1:end-1, 1, next) - correctionFuw(1:end-1);
            w(end, 1, next) = w(end, 1, next) ...
                - .5*(correctionFwz + correctionFuw(end)) ...
                + correctionFvw;
            
            z(1, 1, next) = z(1, 1, next) ...
                + .5*(correctionFvz + correctionFwz) ...
                - correctionFuz;
        else
            % Add the force to u; subtract it from v, w.
            u(end, 1:end-1, next) = u(end, 1:end-1, next) + correctionFuv(1:end-1);
            u(1:end-1, end, next) = u(1:end-1, end, next) + correctionFuw(1:end-1);
            u(end, end, next) = u(end, end, next) + .5*(correctionFuv(end) + correctionFuw(end));

            v(1, :, next) = v(1, :, next) - correctionFuv;
            v(1, end, next) = v(1, end, next) - correctionFvw;

            w(:, 1, next) = w(:, 1, next) - correctionFuw;
            w(end, 1, next) = w(end, 1, next) + correctionFvw;
        end        
    end
    
    %% Get output
    outl = min(max(3, NxFrac*outPosl(n)), Mxu - 1);
    outm = min(max(3, NyFrac*outPosm(n)), Myu - 1);
    outSamp = interpolate2d(u(:, :, next), [outl, outm]);

    if outputMode == 0
        y(n) = outSamp;
    else
        out(1) = outSamp;  
        y(n) = 1e-4 * (out(1) - out(3)) / (2*k);
        out(3) = out(2);
        out(2) = out(1);
    end
    
    %% Store grid dimensions for next iteration
    NxPrev = Nx;
    NyPrev = Ny;
    
    %% Plot the state. Maybe.
    if doDisplacementPlot && n > plotStart && mod(n, plotRate) == 0
        figure(stateFig);
        if plotType == 1
            [xx, yy] = meshgrid((0:Mxu)*h, (0:Myu)*h);
            pcolor(xx, yy, u(:, :, next)');
            hold on;
            [xx, yy] = meshgrid(fliplr(Lx - (0:vWidth)*h), (0:Myu)*h);
            pcolor(xx, yy, v(:, :, next)');

            [xx, yy] = meshgrid((0:Mxu)*h, fliplr(Ly - (0:wWidth)*h));
            pcolor(xx, yy, w(:, :, next)');

            [xx, yy] = meshgrid(fliplr(Lx - (0:vWidth)*h), fliplr(Ly - (0:wWidth)*h));
            pcolor(xx, yy, z(:, :, next)');
            hold off;
        else
            markerSize = 3;
            lineWidth = 1.5;
            [xx, yy] = meshgrid((0:Mxu)*h, (0:Myu)*h);
            plot3(xx, yy, u(:, :, next), 'o-k', 'MarkerSize', markerSize, 'LineWidth', lineWidth);
            grid on;
            ax = gca;
            ax.FontSize = 16;
            xlabel('$L_x, (l)$', 'interpreter', 'latex', 'FontSize', 24); 
            ylabel('$L_y, (m)$', 'interpreter', 'latex', 'FontSize', 24); 
            zlabel('$u_{l,m}$', 'interpreter', 'latex', 'FontSize', 28);
            xlim([0, Lx]);
            ylim([0, Ly]);
            zlim([-plotZLim, plotZLim]);
    %         view(135, 0);

            hold on;
            [xx, yy] = meshgrid(fliplr(Lx - (0:vWidth)*h), (0:Myu)*h);
            plot3(xx, yy, v(:, :, next), 'o-g', 'MarkerSize', markerSize, 'LineWidth', lineWidth);

            [xx, yy] = meshgrid((0:Mxu)*h, fliplr(Ly - (0:wWidth)*h));
            plot3(xx, yy, w(:, :, next), 'o-m', 'MarkerSize', markerSize, 'LineWidth', lineWidth);

            [xx, yy] = meshgrid(fliplr(Lx - (0:vWidth)*h), fliplr(Ly - (0:wWidth)*h));
            plot3(xx, yy, z(:, :, next), 'o-c', 'MarkerSize', markerSize, 'LineWidth', lineWidth);

            scatter3(outl*h, outm*h, outSamp, 128, 'filled', 'r');
            hold off;
        end
        drawnow;
    end
    
    %% Give a clue as to how long this is going to take.
    pctDone = 1000*n/nF;
    prevPctDone = 1000*(n-1)/nF;
    if mod(floor(pctDone), 10) == 0 && mod(floor(prevPctDone), 10) == 9
        if exist('nbytes', 'var')
            fprintf(repmat('\b', 1, nbytes));
        end
        nbytes = fprintf("%d%%", floor(pctDone*.1));
    end
end
disp(' ');
toc

clear indices next curr prev;

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
plot(tVec, NxFracVec, tVec, NyFracVec, 'LineWidth', 2);
xlabel('Time (s)', 'FontSize', fontSize);
ylabel('$N$', 'interpreter', 'latex', 'FontSize', latexFontSize);
% title('Size of spatial domain', 'FontSize', fontSize);
legend('X', 'Y', 'FontSize', 12);
grid on;

figure;
spectrogram(y, 512, 64, 512, Fs, 'yaxis');


%% 3D raised cosine

% inSize = 10;
% cos2D = .5*(1-cos(pi*(0:inSize)/(inSize/2)));
% cos3D = cos2D'*cos2D;
% 
% disp = zeros(10, 10);
% for n=0:10
%     ncos = .5*(1-cos(pi*n/5));
%     for l=0:10
%         lcos = .5*(1-cos(pi*l/5));% * ncos;
%         for m=0:10
%             disp(l+1, m+1) = .5*(1-cos(pi*m/5)) * lcos * ncos;
%         end
%     end
%     
%     mesh(disp);
%     zlim([0, 1]);
%     drawnow;
%     pause(1);
% end
