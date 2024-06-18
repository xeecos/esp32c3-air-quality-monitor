const fs = require("fs");
const { Canvas } = require("skia-canvas");
const args = require('minimist')(process.argv.slice(2));

let w = 1200, h = 400;

if (args['name'])
{
    let str = fs.readFileSync(args['name'], { encoding: "ascii" });   
    let list = str.split("\n").slice(1);
    let canvas = new Canvas(w, h);
    render(canvas, [{ name: "PM1", index: 1,color:"#FF0000" }, { name: "PM2.5", index: 2,color:"#00FF00" }, { name: "PM10", index: 3,color:"#0000FF" }], list, "pm.png");
    render(canvas, [{ name: "Voc", index: 4,color:"#FF0000" }, { name: "Temp", index: 5,color:"#00FF00" }, { name: "Humidity", index: 6,color:"#0000FF" }], list, "voc.png");
}
function render (canvas,labels,list,saveName)
{
        
    const WINDOW_SIZE = args['window'] || 100;
    let ctx = canvas.getContext("2d");
    ctx.fillStyle = "#FFFFFF";
    ctx.fillRect(0, 0, w, h);
    let max_value = 0;
    let time_list = [];
    let data_list = [];
    for (let i = 0; i < labels.length; i++)
    {
        data_list.push([]);
    }
    for (let i = 0; i < list.length; i++)
    {
        let line = list[i];
        let split = line.split("|");
        if (split.length < 3) continue;
        time_list.push(new Date(parseInt(split[0])));
        for (let j = 0; j < labels.length; j++)
        {
            let v = parseFloat(split[labels[j].index]);
            data_list[j].push(v);
            if (max_value < v)
            {
                max_value = v;
            }
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
    ctx.lineTo(w, h - 1);
    for (let i = 1; i < 8; i++)
    {
        ctx.moveTo(w/8*i, 0);
        ctx.lineTo(w/8*i, h-10);
    }
    ctx.stroke();
    ctx.textAlign = "left";
    ctx.fillStyle = "rgba(32,45,67,0.4)";
    ctx.fillText(max_value, 2, 10);
    ctx.fillText(max_value/4*3, 2, h/4);
    ctx.fillText(max_value/2, 2, h/2);
    ctx.fillText(max_value/4, 2, h/4*3);
    ctx.textAlign = "center";
    for (let i = 1; i < 8; i++)
    {
        let ct = time_list[(time_list.length / 8 * i) >> 0];
        ctx.fillText(("0" + ct.getHours()).substr(-2, 2) + ":" + ("0" + ct.getMinutes()).substr(-2, 2), w / 8 * i, h);
    }
    ctx.textAlign = "right";
    ct = time_list[time_list.length - 1];
    ctx.fillText(("0" + ct.getHours()).substr(-2, 2) + ":" + ("0" + ct.getMinutes()).substr(-2, 2), w - 1, h);

    ctx.textAlign = "left";
    for(let n = 0;n<labels.length;n++)
    {
        ctx.fillStyle = labels[n].color;
        ctx.beginPath();
        ctx.arc(6, h - 26 + n*10, 4, 0, Math.PI * 2);
        ctx.fill();
        ctx.fillText(labels[n].name, 12, h - 22.0 + n*10);
        ctx.lineWidth = 1;
        ctx.strokeStyle = labels[n].color;
        ctx.beginPath();
        ctx.moveTo(0, h - smooth(data_list[n], 0, WINDOW_SIZE) / max_value * h);
        for (let i = 1, len = data_list[n].length-1; i < data_list[n].length; i++)
        {
            let v = smooth(data_list[n], i, WINDOW_SIZE);
            ctx.lineTo(i / len * w, h - v / max_value * h);
        }
        ctx.stroke();
    }
    canvas.saveAsSync(saveName);
}
function smooth (list, index, window_size)
{
    let sum = 0, count = 0;
    for (let i = index - window_size; i <= index + window_size; i++)
    {
        if (i < 0 || (i > list.length-1))
        {
            continue;
        }
        sum += list[i];
        count++;
    }
    return sum / count;
}