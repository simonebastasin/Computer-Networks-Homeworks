%% GENERATION OF INPUT DATA file for HW1

clear all
% file name
namefile = 'input.data'; 
% number of bytes for each codepoint
nbyte = 4;
% precision of the data
codepointlength = strcat('uint', num2str(nbyte*8));
% total number of codepoints
totchars = 4000;

% probability distribution of codepoint lengths
pp = [0.55 0.2 0.15 0.1]';
%pp = [0.95 0.05 0  0]';
cdfcp = cumsum(pp);

% memchar=zeros(totchars,1);
% memchar=uint32(memchar);
delete(namefile)
fid = fopen(namefile, 'w+');
m=0;
for nchar = 1:totchars
    pickchar = nbyte + 1 - sum(rand<cdfcp); 
    codepoint = 0;
    for j = 1:pickchar
        codepoint = codepoint + 256^(j-1)*(randi(256,1)-1);
    end
    codepoint = uint32(codepoint);
    count = fwrite(fid, codepoint, codepointlength);
    m = max(codepoint, m);
    %memchar(nchar) = codepoint;
end
% 
fclose(fid);
% 
% rmemchar = memchar;
% fid = fopen(namefile, 'r');
% for nchar = 1:totchars
%     codepoint = fread(fid, 1, codepointlength);
%     codepoint = uint32(codepoint);
%     rmemchar(nchar) = codepoint;
% end
% 
% fclose(fid);
