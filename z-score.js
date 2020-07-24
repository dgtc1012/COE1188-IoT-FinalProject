var aggwindow = context.get('aggwindow')||[];

aggwindow.push(msg.payload);
var mean = 0;
var sd = 0;
var sum = 0;
var variance = 0;
if(aggwindow.length > 40){
    sum = 0;
    var n = aggwindow.length;
    var i;
    for(i = 0; i < n; i++){
        sum = sum + parseInt(aggwindow[i]);
    }
    mean = sum/n;
    var sqrd = [];
    for(i = 0; i < n; i++){
        sqrd.push(Math.pow(mean-parseInt(aggwindow[i]),2));
    }
    
    variance = 0;
    for(i = 0; i < n; i++){
        variance = variance + sqrd[i];
    }
    sd = Math.sqrt(variance);
    aggwindow.shift();
    msg.zscore = (mean - msg.payload)/(sd);
}
context.set('aggwindow', aggwindow);

msg.payload = msg.payload + ':' + msg.zscore + ':' + aggwindow.length + ':' + mean + ':' + sd; 
return msg;