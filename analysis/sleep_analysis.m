clc; close all; 

% Load a dataset
load('synthetic_sleep.mat')


% sleep_data is Nx3, being the columns [time, sensor position, pressure]
time = (sleep_data(:,1) + initial_time) / 3600; % Convert to hours after mid-night
pos = sleep_data(:,2);   % 0,1,2,3 = Left, Center Left, Center Right, Right
pressure = sleep_data(:,3); 

% Define for each sensor (roughtly) the same colors of the Arduino IDE Serial Plotter
colorMap = [
    0.0 0.4 0.7; % blue
    0.8 0.2 0.1; % red
    0.2 0.6 0.1; % green
    0.9 0.7 0.1; % yellow
];
colors = colorMap(pos+1, :);

% Plot recorded data
figure;
scatter(time, pressure, 20, colors);
grid on; hold on;
xlabel("Time (hours after mid-night)")
ylabel("Pressure")
ylim([-2000 2000])

% Vertical lines for labels
for k = 1:length(labels_time)
    xline(labels_time(k), 'k', 'LineWidth', 1.2);
    text(labels_time(k), 2000, labels{k}(1),'HorizontalAlignment','center', ...
        'VerticalAlignment','bottom', 'Color','k','FontWeight','bold');
end

% Legend for labels
annotation('textbox', [0.6 0.72 0.2 0.2], ...
    'String', labels, 'FitBoxToText', 'on', 'BackgroundColor', 'white', 'EdgeColor', 'black');

% Color legend (Left, Center Left, Center Right, Right)
for n = 1:4 
    h(n) = plot(nan, nan, 'o', 'MarkerEdgeColor', colorMap(n,:));
end

legend(h, {'Left Side (wall)', 'Center Left', 'Center Right', 'Right Side'}, 'Location', 'southeast');

hold off;


% Histograms by sensor 
% figure;
% labels = {'Left','Center Left','Center Right','Right'};
% for n = 0:3 
%     subplot(1,4,n+1) 
%     histogram(pressure(pos == n), 'FaceColor', colorMap(n+1,:), 'EdgeColor', 'none'); 
%     ylim([0 50]); xlim(1000*[-1.5 1.5]);% xticks([-1000 0 1000])
%     title(labels{n+1}); 
%     xlabel('Pressure value'); ylabel('Frequency'); grid on; 
% end
% 


