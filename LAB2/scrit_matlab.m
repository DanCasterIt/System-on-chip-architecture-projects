clear
clc
x = [2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056, ...
     3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909, ...
     599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647];
numel(x)
y = 0:((2*pi)/32):(2*pi)-((2*pi)/32);
t = square(y);
t = ((t+max(t))/(2*max(t)))*hex2dec('FFF');
numel(t)
%plot(t);
%plot(x);
M = vec2mat(t, 8)
H = vec2mat(x, 8);
numel(M)
F = string(M);
F(:, :, 1) = F(:, :, 1)+','+' ';
H = char(F);
dlmwrite('values.txt', H,'delimiter','');