let startPoint = {x: 0.7, y: 0.3};

let width  = 435;
let height = 435;

let bounds = {x0: -1.5, x1: 1.5, y0: -1.5, y1: 1.5};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

console.assert(width > 0 && height > 0);
console.assert(bounds.x0 < bounds.x1 && bounds.y0 < bounds.y1);

let canvas = document.createElement("canvas");
canvas.setAttribute("width" , width );
canvas.setAttribute("height", height);
canvas.setAttribute("style",  `border: dashed grey 2px`);
document.head.innerHTML = "";
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

function add (p0,p1) { return new Point(p0.x+p1.x, p0.y+p1.y); }
function mul (scl,p) { return new Point(scl*p.x, scl*p.y); }

function dot (p0,p1) { return p0.x*p1.x + p0.y*p1.y; }
function dot2(p) { return dot(p,p); }
function len (p) { return Math.sqrt(dot2(p)); }
function norm(p) { return mul(1/len(p), p); }

function polyEval(P, x) {
	let result = 0;
	let xi = 1;
	for (coeff of P) {
		result += coeff * xi;
		xi *= x;
	}
	return result;
}

function polyDeriv(P) {
	let result = [];
	for (let i=1; i < P.length; i++) {
		result[i-1] = i * P[i];
	}
	return result;
}

function polyBound(P) {
	let result = -Infinity;
	let leading = P[P.length-1];
	for (let i=0; i < P.length-1; i++) {
		result = Math.max(result,
			Math.pow(Math.abs(P[i]/leading), 1/(P.length-1-i))
		);
	}
	return 2*result;
}

function toScreen(p) {
	let remap = (val,s0,e0,s1,e1) => s1 + (e1-s1) * (val-s0) / (e0-s0);
	return new Point(
		remap(p.x, bounds.x0, bounds.x1, 0, width ),
		remap(p.y, bounds.y0, bounds.y1, height, 0)
	);
}

function drawPoint(p) {
	ctx.beginPath();
	let s = toScreen(p);
	let w = ctx.lineWidth*3;
	ctx.ellipse(s.x, s.y, w, w, 0, 0, 2*Math.PI);
	ctx.fill();
	ctx.closePath();
}

function drawParametric(f, t0, t1) {
	console.assert(t0 <= t1);
	ctx.beginPath();
	for (let t=t0; t < t1; t += 1e-2) {
		let s = toScreen(f(t));
		if (t == t0) { ctx.moveTo(s.x, s.y); }
		else         { ctx.lineTo(s.x, s.y); }
	}
	ctx.stroke();
	ctx.closePath();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

class Orbit {
	constructor(pos, vel, duration=null) {
		this.pos = pos;
		this.vel = vel;
		this.duration = duration;

		this.trace = (t) => new Point(
			this.vel.x*t + this.pos.x,
			-t*t + this.vel.y*t + this.pos.y
		);

		this.traceDeriv = (t) => new Point(
			this.vel.x,
			-2*t + this.vel.y
		);
	}
}

let pos0 = new Point(startPoint.x, startPoint.y);
// if (len(pos0) > 1) { pos0 = norm(pos0); }
let vel0 = new Point(0,0);
let orbits = [];

function draw() {
	ctx.strokeStyle = `#000`;
	drawParametric(
		(t) => { return new Point(Math.cos(t), Math.sin(t)); },
		0, 2*Math.PI
	);

	ctx.fillStyle = `#F00`;
	drawPoint(pos0);

	ctx.strokeStyle = `#F00`;
	for (let orbit of orbits) {
		// if (orbit.duration == null) { continue; }
		drawParametric(
			orbit.trace,
			0, (orbit.duration != null) ? orbit.duration : 10
		);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ //

function rootSolver(P) {
	let dP = polyDeriv(P);
	let N = (x) => x - polyEval(P, x) / polyEval(dP, x);

	let result = Infinity;
	for (let x=0; x < polyBound(P); x += 1e-1) {
		let x0 = x;
		for (let i=0; i < 10; i++) { x0 = N(x0); }
		if (Math.abs(x0 - N(x0)) > 1e-6) { continue; }
		if (x0 < 1e-10 || isNaN(x0)) { continue; }
		result = Math.min(result, x0);
	}
	
	return (result != Infinity) ? result : null;
}

orbits.push(new Orbit(pos0, vel0));

for (let i=0; i < 10; i++) {
	let last = orbits[orbits.length-1];
	if (i >= orbits.length) {
		let nextPos = last.trace(last.duration);
		let hitVel = last.traceDeriv(last.duration);

		let normal = norm(mul(-1, nextPos));
		let nextDir = add(hitVel, mul(-2*dot(hitVel, normal), normal));
		let nextSpeed = len(hitVel);
		let nextVel = mul(nextSpeed, norm(nextDir));

		orbits.push(new Orbit(nextPos, nextVel));
	}

	last = orbits.pop();
	last.duration = rootSolver(
		[dot2(last.pos)-1,
		 2*dot(last.pos, last.vel),
		 dot2(last.vel) - 2*last.pos.y,
		 -2*last.vel.y,
		 1]
	);
	orbits.push(last);
	if (last.duration == null) { break; }
}

draw();