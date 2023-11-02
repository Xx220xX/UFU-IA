clc;close all;clear all;


function y = f(v)
  y = v;
  for i = 1:length(v)
    x = v(i);
    if x > 1
      r = log(x+1.35469) -0.0948181139;
    elseif x < -1
      r = -log(-x+1.35469) +  0.0948181139;
    else
      r = tanh(x);
    end
    y(i) = r;
  end
end

function y=df(v)
  y = v;
  for i = 1:length(v)
    x = v(i);
    if x > 1
      r = 1 ./(x+1.35469);
    elseif x < -1
      r = 1 ./(-x+1.35469);
    else
      r = 1/cosh(x).^2;
    end
    y(i) = r;
  end
end


x = linspace(-10,10,100);


y = f(x);
dy = df(x);
t = tanh(x);
hold on;
plot(x,y);
plot(x,dy);
plot(x,t);
grid on;
legend('y','dy')

