var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

var getWiki = function(query){
  
  query = JSON.parse(query)['query']
  
  console.log("query:" + query);
  url = 'http://192.241.218.61/get?q=' + query;  
  
  xhrRequest(url, 'GET', 
        function(responseText) {
                    
          if(responseText !== "Error")
            var json = JSON.parse(responseText);
          else
            return false;
          
          var string = '';
          for(var x in json){
            string += json[x] + '`';
          }
          
          console.log('string: ' + string);
          
          // Assemble dictionary using our keys
          var dictionary = {
            "query": query,
            "steps": string
          };
    
          // Send to Pebble
          Pebble.sendAppMessage(dictionary,
            function(e) {
              console.log("Info sent to Pebble successfully!");
            },
            function(e) {
              console.log("Error info to Pebble!");
            }
          );
        }      
      );
};

Pebble.addEventListener('appmessage',
  function(e) {
    
    console.log("AppMessage received! Got " + JSON.stringify(e.payload));
    
    getWiki(JSON.stringify(e.payload));
  }
);

