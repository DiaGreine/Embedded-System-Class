var token = "";
var temp = msg.payload.Temp;

if (temp <= 50) {
    return;
} else {
    msg.url = "https://notify-api.line.me/api/notify";
    msg.headers = {'content-type':'application/x-www-form-urlencoded','Authorization':'Bearer '+ token};
    var message = "The temperature exceeds 50, current Temp: " + temp;
    msg.payload = {"message":message};
    return msg;
}

