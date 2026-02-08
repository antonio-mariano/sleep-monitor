% Read the CSV (or text) file with 3 columns:
% Time (string), Position, Pressure
data = readtable("events.csv", "Delimiter", ",", "ReadVariableNames", true);

% Convert the timestamp column to MATLAB datetime
t = datetime(data.Time_min_, "InputFormat", "yyyy-MM-dd HH:mm:ss");
t0 = t(1); % Store the absolute initial timestamp (first row)
time_seconds = seconds(t - t0); % seconds elapsed since Pico power on

% Build the sleep_data array (N x 3)
sleep_data = [time_seconds, data.Position, data.Pressure];

% Compute initial_time = seconds after midnight for the first timestamp
initial_time = seconds(timeofday(t0));

if initial_time > 12*3600
    initial_time = initial_time - 24*3600;
end

labels = {'A - example label 1', 'B - example label 2'};
labels_time = [0 1];

% Give a name to the file being saved
output_name = 'example_name.mat';
save(output_name, 'sleep_data', 'initial_time', 'labels', 'labels_time');

