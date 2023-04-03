let width  = 500;
let height = 500;

let bounds = {x0: -1.5, x1: 1.5, y0: -1.5, y1: 1.5};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

console.assert(width > 0 && height > 0);
console.assert(bounds.x0 < bounds.x1 && bounds.y0 < bounds.y1);

let canvas = document.createElement("canvas");
canvas.setAttribute("width" , 500);
canvas.setAttribute("height", 500);
canvas.setAttribute("style",  `border: dashed grey 2px`);
document.body.innerHTML = "";
document.body.appendChild(canvas);

const ctx = canvas.getContext("2d");;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

class Point {
	constructor(x,y) {
		this.x = x;
		this.y = y;
	}
}

function toScreen(p) {
	return new Point();
}

function drawParametric(f, t0, t1) {
	console.assert(t0 <= t1);
	for (let t=t0; t < t1; t += 1e-2) {
		p = f(t);
		if (t == t0) { ctx.moveTo(p.x, p.y); }
		else         { ctx.lineTo(p.x, p.y); }
	}
	ctx.stroke();
}

function draw() {
	drawParametric(
		(t) => { return new Point(Math.cos(t), Math.sin(t)); },
		0, 2*Math.PI
	);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

/* math code */