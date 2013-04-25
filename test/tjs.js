console.log("----------------------");
var arr = [];
for(var i=0;i<5000;i++) {
    arr[i] = i;
}
var myDate = new Date();
var start = myDate.getTime();
for(var i=0;i<5000;i++) {
    var min = arr[i];
    for(var j=i;j<5000;j++) {
          if(min>arr[j]){
               min=arr[j];
               arr[j] = arr[i];
               arr[i] = min;
          }
    }
}
myDate = new Date();
var end = myDate.getTime();
console.log(end - start);
