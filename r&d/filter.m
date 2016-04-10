clear; clf

%user data
fig_plot = 1;         %use any value except 0 or [] to plot figures
s_FIR    = 1280;      %length of the fir (samples)
s_win    = 2 * s_FIR; %window size (samples) for zero padding

[DAFx_in, FS] = wavread('input.wav');

%initialise windows, arrays, ...
L        = length(DAFx_in);
pad = zeros(s_win - mod(L, s_FIR),1);
DAFx_in  = [DAFx_in; pad]...
    / max(abs(DAFx_in)); %0 pad and normalize
DAFx_out = zeros(length(DAFx_in) + s_FIR,1);
grain    = zeros(s_win, 1); %input grain
vec_pad  = zeros(s_FIR, 1); %padding array

%initialise calculation of fir
x     = (1:s_FIR);
fr    = 1000 / FS;
alpha = -0.002;
fir   = (exp(alpha*x).*sin(2*pi*fr*x))'; % fir coefficients
fir2  = [fir; zeros(s_win - s_FIR, 1)];
fcorr = fft(fir2);

%displays the filter impulse response
if (fig_plot)
    figure(1); clf;
    subplot(2,1,1); plot(fir); xlabel('n [samples]');
    ylabel('h(n)'); axis tight;
    title('Impulse response of the FIR')
    subplot(2,1,2);
    plot((0:s_FIR-1)/s_FIR*FS, 20 *log10(abs(fft(fftshift(fir)))));
    xlabel('k'); ylabel('|F(n,k) / db');
    title('Magnitude spectrum of the FIR'); axis([0 s_FIR/2, -40, 50])
end

%processing
tic
pin  = 0;
pout = 0;
pend = length(DAFx_in) - s_FIR;

while pin < pend
    grain = [DAFx_in(pin + 1 : pin + s_FIR); vec_pad];
    
    ft    = fft(grain) .*fcorr;
    grain = (real(ifft(ft)));
    
    DAFx_out(pin+1:pin + s_win) = DAFx_out(pin+1:pin + s_win) + grain;
    pin   = pin + s_FIR;
end

%listening and saving the output
DAFx_out = DAFx_out / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'output.wav');






