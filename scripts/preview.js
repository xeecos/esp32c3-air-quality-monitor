const fs = require("fs");
const { Canvas } = require("skia-canvas");
const args = require('minimist')(process.argv.slice(2))
if (args['name'])
{
    let str = fs.readFileSync(args['name'], { encoding: "ascii" });   
    let list = str.split("\n").slice(1);
    let w = 1000, h = 400;
    const WINDOW_SIZE = 100;
    let canvas = new Canvas(w, h);
    let ctx = canvas.getContext("2d");
    ctx.fillStyle = "#FFFFFF";
    ctx.fillRect(0, 0, w, h);
    let time_list = [];
    let pm1_list = [];
    let pm2_list = [];
    let pm10_list = [];
    let max_value = 0;
    for (let i = 0; i < list.length; i++)
    {
        let line = list[i];
        let split = line.split("|");
        time_list.push(new Date(parseInt(split[0])));
        pm1_list.push(parseInt(split[1]));
        pm2_list.push(parseInt(split[2]));
        pm10_list.push(parseInt(split[3]));
        if (max_value < pm1_list[i])
        {
            max_value = pm1_list[i];
        }
        if (max_value < pm2_list[i])
        {
            max_value = pm2_list[i];
        }
        if (max_value < pm10_list[i])
        {
            max_value = pm10_list[i];
        }
    }
    
    ctx.strokeStyle = "rgba(32,45,67,0.1)";
    ctx.lineWidth = 0.4;
    ctx.beginPath();
    ctx.moveTo(0, 1);
    ctx.lineTo(w, 1);
    ctx.moveTo(0, h/4);
    ctx.lineTo(w, h/4);
    ctx.moveTo(0, h/2);
    ctx.lineTo(w, h/2);
    ctx.moveTo(0, h/4*3);
    ctx.lineTo(w, h/4*3);
    ctx.moveTo(0, h-1);
    ctx.lineTo(w, h-1);
    ctx.moveTo(w/4, 0);
    ctx.lineTo(w/4, h);
    ctx.moveTo(w/2, 0);
    ctx.lineTo(w/2, h);
    ctx.moveTo(w/4*3, 0);
    ctx.lineTo(w/4*3, h);
    ctx.stroke();
    ctx.textAlign = "left";
    ctx.fillStyle = "rgba(32,45,67,0.4)";
    ctx.fillText(max_value, 2, 10);
    ctx.fillText(max_value/4*3, 2, h/4);
    ctx.fillText(max_value/2, 2, h/2);
    ctx.fillText(max_value/4, 2, h/4*3);
    ctx.textAlign = "center";
    let ct = time_list[(time_list.length / 2) >> 0];
    ctx.fillText(ct.getHours() + ":" + ("0" + ct.getMinutes()).substr(-2, 2), w / 2, h);
    ct = time_list[(time_list.length / 4) >> 0];
    ctx.fillText(ct.getHours() + ":" + ("0" + ct.getMinutes()).substr(-2, 2), w / 4, h);
    ct = time_list[(time_list.length / 4*3) >> 0];
    ctx.fillText(ct.getHours() + ":" + ("0" + ct.getMinutes()).substr(-2, 2), w / 4 * 3, h);
    ctx.textAlign = "left";
    ctx.fillStyle = "#FF0000";
    ctx.beginPath();
    ctx.arc(6, h - 26, 4, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillText("PM1.0", 12, h - 22.5);
    ctx.fillStyle = "#00FF00";
    ctx.beginPath();
    ctx.arc(6, h - 16, 4, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillText("PM2.5", 12, h - 12.5);
    ctx.fillStyle = "#0000FF";
    ctx.beginPath();
    ctx.arc(6, h - 6, 4, 0, Math.PI * 2);
    ctx.fill();
    ctx.fillText("PM10", 12, h - 2.5);
    ctx.strokeStyle = "#FF0000";
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.moveTo(0, h - smooth(pm1_list, 0, WINDOW_SIZE) / max_value * h);
    for (let i = 1, len = pm1_list.length-1; i < pm1_list.length; i++)
    {
        let v = smooth(pm1_list, i, WINDOW_SIZE);
        ctx.lineTo(i / len * w, h - v / max_value * h);
    }
    ctx.stroke();
    ctx.strokeStyle = "#00FF00";
    ctx.beginPath();
    ctx.moveTo(0, h - smooth(pm2_list, 0, WINDOW_SIZE) / max_value * h);
    for (let i = 1, len = pm2_list.length-1; i < pm2_list.length; i++)
    {
        let v = smooth(pm2_list, i, WINDOW_SIZE);
        ctx.lineTo(i / len * w, h - v / max_value * h);
    }
    ctx.stroke();
    ctx.strokeStyle = "#0000FF";
    ctx.beginPath();
    ctx.moveTo(0, h - smooth(pm10_list, 0, WINDOW_SIZE) / max_value * h);
    for (let i = 1, len = pm10_list.length-1; i < pm10_list.length; i++)
    {
        let v = smooth(pm10_list, i, WINDOW_SIZE);
        ctx.lineTo(i / len * w, h - v / max_value * h);
    }
    ctx.stroke();
    canvas.saveAsSync("preview.png");
}
function smooth (list, index, window_size)
{
    let sum = 0, count = 0;
    for (let i = index - window_size; i <= index + window_size; i++)
    {
        if (i < 0 || i >= list.length)
        {
            continue;
        }
        sum += list[i];
        count++;
    }
    return sum / count;
}