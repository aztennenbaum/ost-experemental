function [ S ] = sparse_diag( V )

S=sparse(1:numel(V),1:numel(V),V);

end

