Pebble.addEventListener("ready",
      function(e) {
            console.log("JavaScript app ready and running!");
                Pebble.showSimpleNotificationOnPebble("now", "you know");
                  }
    );

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL("http://google.com");
});
