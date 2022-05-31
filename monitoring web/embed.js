let light1 = false;
let light2 = false;
var people; // need to get value from database

function updateState(data){ // data 3 bit ; first bit check people enter(1)/exit(2)/nothing(0) , second bit check light 1 on(1)/off(0) , third bit check light 2 on(1)/off(0)
  if(data[0] == 1) people +=1;
  else if(data[0]==2){
    people -=1;
    if(people<0) people = 0;
  }
  if(data[1]==1) updateLight1(true);
  else if(data[1]==0) updateLight1(false);
  if(data[2]==1) updateLight2(true);
  else if(data[2]==0) updateLight2(false);
}

function updateLight1(state){
    if(state && !light1 || !state && light1){
      lightToggle(1);
    }
}

function updateLight2(state){
  if(state && !light2 || !state && light2){
    lightToggle(2);
  }
}

function lightToggle(n){
  if(n===1){ // toggle light 1
    light1 = !light1;
    document.getElementById("button_box_1").classList.toggle("press");
  }else{ // toggle light 2
    light2 = !light2;
    document.getElementById("button_box_2").classList.toggle("press");
  }
}

function updatePeopleCount(n){
  document.getElementById("people_count").innerHTML = n;
}

//------------------------------------------------------------------ backend
function isJson(str) {
  try {
      JSON.parse(str);
  } catch (e) {
      return false;
  }
  return true;
}

// API , get data from database(netpie)
var url = "https://api.netpie.io/v2/device/shadow/data/json";
var xhr = new XMLHttpRequest();
xhr.open("GET", url);
xhr.setRequestHeader("Authorization", "Basic Y2M0NGExMWItYjA5ZS00OTg0LWE1NzYtYTg0MmM4MjhjZTQ2OjJUdldCU1hhcnl3dXpTZjZ5TjQyQ0tkUWVTblVmRUU5");
xhr.onreadystatechange = function () {
   if (xhr.readyState === 4) {
      console.log(xhr.status);
      console.log(xhr.responseText);
      const myJSON = xhr.responseText;
      const myObj = JSON.parse(myJSON);
      people = myObj["data"]["people"];
      updatePeopleCount(people);
      console.log("people = " + people);
      if( myObj["data"]["light1"] == "on"){
        lightToggle(1);
      }
      if( myObj["data"]["light2"] == "on"){
        lightToggle(2);
      }
   }};
xhr.send();

client = new Paho.MQTT.Client("mqtt.netpie.io", 443, "028a21b8-2b9d-412c-b826-7cebffb310da");
var options = {
    useSSL: true,
    userName: "WRCxJnWkiDEyfLXQeygVFTNf1w8pLDwK",
    password: "EuZgHTt(V7E6u~IL)qmuWj(_folmz4-i",
    onSuccess:onConnect,
    onFailure:doFail
}
client.connect(options);

function onConnect() {
    console.log("connected");
    // client.subscribe("@msg/people");
    client.subscribe("@msg/feedback");
    // mqttSend("@msg/led", "GET");
}

function doFail(e){
    console.log("failed : " + e);
}

client.onMessageArrived = function(message) {
    console.log("message arrived: " + message.payloadString);
    data = message.payloadString;
    console.log("data arrived: " + data);
    updateState(String(data));
    updatePeopleCount(people);
}
  
// var mqttSend = function(topic, msg) {
//   var message = new Paho.MQTT.Message(msg);
//   message.destinationName = topic;
//   client.send(message);
// }

// function send_update(n){
//   if(n==1){
//     if(light1){
//       mqttSend("@msg/led_control", "3"); // light 1 on
//     }
//     else{
//       mqttSend("@msg/led_control", "4"); // light 1 off
//     }
//   }
//   else{
//     if(light2){
//       mqttSend("@msg/led_control", "5"); // light 2 on 
//     }
//     else{
//       mqttSend("@msg/led_control", "6"); // light 2 off 
//     }
//   }
//   // mqttSend("@msg/people", people + ";");
// }

//------------------------------------------------------------------ backend
