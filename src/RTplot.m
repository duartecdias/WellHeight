x=1:100;
y=zeros(1,100);
count = 1;
figure(1);
ylim([200,300]);
hold on;
h = [];
while 1,
    strNum = urlread('http://192.168.0.106');
    y = [str2num(strNum) y];
    y = y(1:end-1);
    count = count + 1;
    if count > 1
        count = 1;
        delete(h);
        h = plot(x, y);
        drawnow;
    end
    pause(0.05);
end
