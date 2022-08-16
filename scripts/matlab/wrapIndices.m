function indices = wrapIndices(indices, bufferLength)
%WRAPINDICES Wrap sample indices that fall outside of the length of a buffer.
    for i=1:length(indices)
        if indices(i) < 1
            indices(i) = bufferLength + indices(i);
        elseif indices(i) > bufferLength
            indices(i) = mod(indices(i), bufferLength);
        end
    end
end