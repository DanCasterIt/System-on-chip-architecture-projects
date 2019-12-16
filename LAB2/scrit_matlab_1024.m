clear
clc
unit = ((2*pi)/1024);
y = 0:unit:(2*pi)-unit;
t = sin(y);
t = ((t+max(t))/(2*max(t)))*hex2dec('FFF');
numel(t)
t = int16(t);
plot(t);
M = vec2mat(t, 16)
numel(M)
F = string(M);
F(:, :, 1) = F(:, :, 1)+','+' ';
H = char(F);
dlmwrite('values_1024.txt', H,'delimiter','');