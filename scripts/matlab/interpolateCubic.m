function y = interpolateCubic(x, readPos, doPlot)
    if nargin == 2
        doPlot = false;
    end
    % Get the indices of the elements we need from the input.
    % Start by getting the index of the ith element.
    n = floor(readPos);
    % Create a vector of the four indices we need for cubic interpolation. 
    readIndices = [n-1, n, n+1, n+2];
    % Get the fractional part of the read index.
    alpha = mod(readPos, 1);
    % Calculate the coefficients.
    % coeffs(1) is the coefficient for the (n-1)th sample
    % coeffs(2) => nth sample
    % coeffs(3) => (n+1)th sample
    % coeffs(4) => (n+2)th sample
    coeffs = [ ...
        -alpha * (alpha - 1) * (alpha - 2) / 6; ...
        (alpha - 1) * (alpha + 1) * (alpha - 2) / 2; ...
        -alpha * (alpha + 1) * (alpha - 2) / 2; ...
        alpha * (alpha + 1) * (alpha - 1) / 6 ...
    ];

    if doPlot && mod(floor(readPos*100), 5) == 0
        plot([zeros(1, n-2), coeffs', zeros(1, length(x) - (n-2) - 4)]); drawnow limitrate;
%         plot(x .* [zeros(1, n-2), coeffs', zeros(1, length(x) - (n-2) - 4)]); drawnow limitrate;
    end

    y = x(readIndices) * coeffs;
end