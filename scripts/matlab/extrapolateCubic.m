function x = extrapolateCubic(x, writePos, gridSpacing, value)
    % Get the indices of the elements we need from the input.
    % Start by getting the index of the ith element.
    n = floor(writePos);
    % Create a vector of the four indices we need for cubic extrapolation. 
    % Wrap as required.
    writeIndices = [n-1, n, n+1, n+2];

    % Get the fractional part of the write position.
    alpha = mod(writePos, 1);
    % Calculate the coefficients.
    % coeffs(1) is the coefficient for the (n-1)th sample
    % coeffs(2) => nth sample
    % coeffs(3) => (n+1)th sample
    % coeffs(4) => (n+2)th sample
    % Since this is an extrapolation, divide all the coeffcients by the grid-
    % spacing.
    coeffs = [ ...
        -alpha * (alpha - 1) * (alpha - 2) / 6, ...
        (alpha - 1) * (alpha + 1) * (alpha - 2) / 2, ...
        -alpha * (alpha + 1) * (alpha - 2) / 2, ...
        alpha * (alpha + 1) * (alpha - 1)/6 ...
    ] / gridSpacing;

    x(writeIndices) = x(writeIndices) + (value * coeffs);
end