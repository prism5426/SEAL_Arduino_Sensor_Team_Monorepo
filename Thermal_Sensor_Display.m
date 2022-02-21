% PREPROCESS
% Resets all ports.
instrreset 
close all
clearvars
clc

% Input desired Arduino board port along with baud rate.
ser = serialport('COM3',115200); 

% INSTRUCTIONS: Press 'Run'
% Pixel matrix dimensions
row_len = 8;
col_len = 8;
pixel_matrix = zeros(row_len, col_len);


% Parses serial output from Arduino Nano.
while true
    out = readline(ser);
    data_str = split(out, ",");
    count = 1;
    if length(data_str) == 64 && count < 65
        for i = 1:row_len
            for j = 1:col_len
                pixel_matrix(i,j)= data_str(count);
                count = count + 1;
            end
        end
    end
    plot_Display(pixel_matrix)
end

%{ 
Display of thermal heat map with bilinear interpolation.The output pixel value is a 
weighted average of pixels in the nearest 2-by-2 neighborhood. Furthermore,
the image is resized by 30 times, from a 8 x 8 to 240 x 240 matrix. Feel
free to calibrate.
%}
function plot_Display(pixel_matrix)
    pixel_new = imresize(pixel_matrix, 30,'bilinear');
    pcolor(pixel_new)
    shading interp
    %{ 
    c = colorbar; Dynamic colorbar causes lag.
    c.Label.String = 'Tepmerature (C)';
    shg
    %}
end




