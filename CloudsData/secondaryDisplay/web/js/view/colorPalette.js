ColorPalette = {
	baseColors: new Array(),
	originalColors: new Array(), //want to know how many different colors there are, and map them to my palette
	init: function(){
		//Palettes from reference
		//HSV
		//PALLETE 1	
		//H btw 203.88 and 204
		//S btw 9.85 and 40.55
		//V btw 49.8 and 99.61; 99.61 for whiteish; btw 49.41 and 79.61 for darker grays.
		//this.baseColors.push(tinycolor("hsv 203.88, 40.55%, 99.61%"));
		//this.baseColors.push(tinycolor("hsv 200, 9.52%, 49.41%"));//gray
		//this.baseColors.push(tinycolor("hsv 204, 9.84%, 99.61%"));//light gray
		//this.baseColors.push(tinycolor("hsv 204.7, 40.16%, 49.8%"));
		//this.baseColors.push(tinycolor("hsv 204, 9.85%, 79.61%"));//med gray
		
		//PALLETE 2 – TURQUOISES 
		//H between 195 and 196
		//S between 20 and 90.55
		//V between 90 and 99.61 for color; 29.41 for dark gray.
		this.baseColors.push(tinycolor("hsv 195.88, 20.16%, 99.61%"));
		this.baseColors.push(tinycolor("hsv 195.65, 90.55%, 49.8%"));
		this.baseColors.push(tinycolor("hsv 195.13, 90.55%, 99.61%"));
		//this.baseColors.push(tinycolor("hsv 192, 13.33%, 29.41%"));//dark gray
		this.baseColors.push(tinycolor("hsv 195.65, 90.2%, 80%"));
		
		//PALETTE 3 - BLUES
		//H between 210 and 210.7
		this.baseColors.push(tinycolor("hsv 210, 96.45%, 55.29%"));
		this.baseColors.push(tinycolor("hsv 210.5, 93.23%, 75.29%"));
		this.baseColors.push(tinycolor("hsv 210.7, 72.43%, 95.29%"));
		//this.baseColors.push(tinycolor("hsv 210.7, 0%, 94.51%"));//light gray
		//this.baseColors.push(tinycolor("hsv 210, 03%, 14.51%"));//dark gray
		
		
	},
	getRandomColor: function(){
		var pos = Math.floor(Math.random() * this.baseColors.length);
		return this.baseColors[pos];
	},
	addOriginalColor: function(color){
		if(this.originalColors.indexOf(color) < 0){
			this.originalColors.push(color);
		}
	},
	clamp: function(val) {
	    return Math.min(1, Math.max(0, val));
	},
	getPaletteColor: function(originalColor){
		var posInOriginalColors = this.originalColors.indexOf(originalColor);
		//map to a pos in the palette array
		var newColorPos = Math.floor(posInOriginalColors/this.originalColors.length * this.baseColors.length);		
		var newColor = this.baseColors[newColorPos].toHsl();
		//randomly lighten or darken to create sense of depth
		newColor.h = (newColor.h + this.random()*20) % 360;//change range from (0, 1) to (-1, 1) first
		newColor.l += this.random() * 0.4; 
		newColor.l = ColorPalette.clamp(newColor.l);
		newColor.s += this.random() * 0.5;
		newColor.s = ColorPalette.clamp(newColor.s);
		return tinycolor(newColor);
	},
	random: function(){//from -1 to 1
		return Math.random()*2 - 1;
	}
	
	
}