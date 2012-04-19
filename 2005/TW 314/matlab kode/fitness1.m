function fitness1(lang,key,teks)
N = size(teks,2);
p = fluct(teks);
for i=1:26
    expect(i) = let_freq(lang,mZ(i-1));
end
plot(1:26,p)
expect = expect/sum(expect)*N
p = p*N
f = ( 2*(N-min(p)) - sum( abs(p - expect) ) )/(2*(N-min(p)));
disp(['Fitness measure = ',num2str(f)]);