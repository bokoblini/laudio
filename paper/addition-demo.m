f = [zeros(1,50), ones(1, 250), zeros(1, 250)];
fftf = fft(f);
components = zeros(550, 550);
n = 0:549;

for i = 1:550
	components(i,:) = abs(fftf(i)) * cos(2*pi*(i-1)/549*n + arg(fftf(i)));
endfor

sums = zeros(550,550);
curr = zeros(1, 550);

for i = 1:550
	curr = curr + components(i,:);
	sums(i,:) = curr;
endfor

idx = 1:550;

subplot(6,1,1)
plot(idx, sums(2,:), idx, f(idx)*250+125)
subplot(6,1,2)
plot(idx, components(4,:), idx, sums(4,:), idx, f(idx)*250+125)
subplot(6,1,3)
plot(idx, components(6,:), idx, sums(6,:), idx, f(idx)*250+125)
subplot(6,1,4)
plot(idx, components(8,:), idx, sums(8,:), idx, f(idx)*250+125)
subplot(6,1,5)
plot(idx, components(10,:), idx, sums(10,:), idx, f(idx)*250+125)
subplot(6,1,6)
plot(idx, sums(2,:), idx, sums(4,:), idx, sums(6,:), idx, sums(8,:), idx, sums(10,:), idx, sums(12,:), idx, sums(250,:))
