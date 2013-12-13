
Raphael.fn.zoom = function(scaleFactor){	
	var newWidth = this._viewBox[2] / scaleFactor;
	var newHeight = this._viewBox[3] / scaleFactor;
	
	//newX = (newWidth - oldWidth) / 2 - oldX (draw it out!)
	var newX = ((newWidth -  this._viewBox[2]) / 2 - this._viewBox[0]) - newWidth;
	var newY = ((newHeight -  this._viewBox[3]) / 2 - this._viewBox[1]) - newHeight;
	
	this.setViewBox(newX, newY, newWidth, newHeight, false);
	
},
Raphael.fn.pan = function(panX, panY){ //pan: either 1 or -1.
	var newX = parseInt(this._viewBox[0]) + panX;
	var newY = parseInt(this._viewBox[1]) + panY;
	this.setViewBox(newX, newY, this._viewBox[2], this._viewBox[3]);
},

Raphael.fn.animateViewBox = function animateViewBox( x, y, w, h, duration, easing_function, callback )
{	
	
	var cx, cy, dy, dw, dh;
	var self = this;
	if(this._viewBox){
		cx = this._viewBox[0];
		cy = this._viewBox[1];
		cw = this._viewBox[2];
		ch = this._viewBox[3];
	}
	else{
		cx = 0;
		cy = 0;
		cw = 0;
		ch = 0;
	}
	var dx = x - cx;
	var dy = y - cy;
    var dw = w - cw;
 	var dh = h - ch;
    
    var easing_function = easing_function || "linear";


    var interval = 25;
    var steps = duration / interval;
    var current_step = 0;
    var easing_formula = Raphael.easing_formulas[easing_function];
    //Raphael.fn.intervalID = setInterval( function()	
	Raphael.fn.intervalID = setInterval( function()	
        {
			//console.log( "cx IN: " + cx);
            var ratio = current_step / steps;
            self.setViewBox( cx + dx * easing_formula( ratio ),
                             cy + dy * easing_formula( ratio ),
                             cw + dw * easing_formula( ratio ),
                             ch + dh * easing_formula( ratio ), false );
            if ( current_step++ >= steps || self.stop)
            {	
                //clearInterval( Raphael.fn.intervalID );
				clearInterval( Raphael.fn.intervalID );
                callback && callback();
            }
			Controller.stats.update();
        }, interval );
}

Raphael.el.pulsate = function(){
        this.animate({ 'stroke-opacity': 0.2 }, 1200, 'linear', function(){
			this.animate({ 'stroke-opacity': 1 }, 1200, 'linear', this.pulsate)
		});        
}

Raphael.el.fadeIn = function(from, to){
	this.attr("opacity", from);
	return this.animate({ opacity: to }, 1200, 'linear');
}

Raphael.el.fadeOut = function(callback){
	return this.animate({ opacity: 0 }, 1200, 'linear', callback);
}

Raphael.el.highlight = function(maxlight){
	var self = this;
	var highlightedColor = tinycolor.lighten(this.attr('fill'), maxlight).toHexString();
    this.animate({ fill: highlightedColor}, 1200, '<');
}

Raphael.el.glowOnce = function(from, to, backTo){	
		this.attr("opacity", from);	
		var self = this;
        this.animate({ opacity: to}, 1200, '<', function(){
			this.animate({ opacity: backTo }, 1200, '>', self.callback)
		});        
}

Raphael.el.stopAnimation = function(){	
     this.stop();
}

    


