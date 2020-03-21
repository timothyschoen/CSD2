
function sliderElement(divComponent) {


  var slider = document.createElement("INPUT");
  slider.type = "range";
  slider.min = 0;
  slider.max = 1;
  slider.step = 0.001;
  slider.style.position = "relative";
  slider.style.width = 155 + "px";
  slider.style.height = "20px"
  slider.style.bottom = "5px";
  slider.style.right = "0px";
  slider.style.resize = "none";
  slider.style.display = "none";
  slider.style.zIndex = "1";

  divComponent.style.width = 185 + "px";
  divComponent.appendChild(slider);


  slider.addEventListener('mousedown',function (event){
   event.stopPropagation();
});


  let valuelabel = document.createElement("div");
  valuelabel.style.cssText = "color:#dcdcdc; font-size:13px; font-family:sans-serif; position:relative; bottom:24px; left:163px".replace('x0', 20);
  valuelabel.innerHTML = parseFloat(slider.value).toFixed(2);
  divComponent.appendChild(valuelabel);

  slider.style.display = "block";
  valuelabel.style.display = "block";


slider.update = function() {
    divComponent.setargs(["/slider" + sliders.indexOf(divComponent), JSON.stringify(sliders.indexOf(divComponent))]);
}

divComponent.update = function(a) {
  slider.update();
}

  slider.oninput = function() {
      valuelabel.innerHTML = parseFloat(slider.value).toFixed(2);
    client.send("/slider"+sliders.indexOf(divComponent), parseFloat(slider.value), (err) => {
      if (err) console.log(err);
    });
    slider.update();
  }


  this.delete = function() {
      sliders.splice(sliders.indexOf(divComponent), 1);
      for (var i = 0; i < sliders.length; i++) {
        sliders[i].update();
      }

      // undo slider specific modifications we made to the component
      divComponent.setargs([]);
      divComponent.style.width = "";
      divComponent.style.paddingTop = "3px";
      divComponent.style.paddingBottom = "1px";
      divComponent.style.paddingRight = "12px";
      divComponent.update = undefined;

      slider.remove();
      valuelabel.remove();

  }
  slider.update();
}
