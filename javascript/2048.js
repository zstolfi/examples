var board;
var score;
var numMoves;
var input;
var gameOver;

function setup() {
	board = new Object();
	board.grid = [];
	for (var i=0; i < 4; i++) {
		board.grid[i] = [0,0,0,0];
	}
	score = 0;
	numMoves = 0;
	input = null;
	gameOver = false;
}
function exit() {
	gameOver = true;
	console.log("Game Over!");
	console.log("Score: " + score);
}

function getInput(e) {
	switch (e.key) {
		case "ArrowUp":
		case "w":
		case "W":
			input = "up";  break;
			
		case "ArrowDown":
		case "s":
		case "S":
			input = "down";  break;
			
		case "ArrowLeft":
		case "a":
		case "A":
			input = "left";  break;
			
		case "ArrowRight":
		case "d":
		case "D":
			input = "right";  break;

		case "Escape":
			input = "exit";  break;
		default:
			input = null;
	}
	return input;
}

function run() {
	if (gameOver || input == null) { return; }

	/* GAMEPLAY */
	if (numMoves > 0) {
		var illegalMoves = [];
		if (shiftGrid("left" )[0] < 1) { illegalMoves.push("left" ); }
		if (shiftGrid("right")[0] < 1) { illegalMoves.push("right"); }
		if (shiftGrid("up"   )[0] < 1) { illegalMoves.push("up"   ); }
		if (shiftGrid("down" )[0] < 1) { illegalMoves.push("down" ); }

		if (illegalMoves.length == 4) { exit(); }
		if (illegalMoves.indexOf(input) != -1) { return; }
		
		var [, addedScore] = shiftGrid(input, true);
		score += addedScore;

		cellSet(randomOpen(), newNumber());
	} else { // first move!
		cellSet(randomOpen(), newNumber());
		cellSet(randomOpen(), newNumber());
	}

	numMoves++;

	/* DRAW THE GRID */
	draw();
}



function shiftGrid(dir, updateMain = false) {
	var lines;
	switch (dir) {
		case "left":
			lines = [
				[[0,0], [0,1], [0,2], [0,3]] ,
				[[1,0], [1,1], [1,2], [1,3]] ,
				[[2,0], [2,1], [2,2], [2,3]] ,
				[[3,0], [3,1], [3,2], [3,3]] ,
			];
			break;
			
		case "right":
			lines = [
				[[0,3], [0,2], [0,1], [0,0]] ,
				[[1,3], [1,2], [1,1], [1,0]] ,
				[[2,3], [2,2], [2,1], [2,0]] ,
				[[3,3], [3,2], [3,1], [3,0]] ,
			];
			break;
			
		case "up":
			lines = [
				[[0,0], [1,0], [2,0], [3,0]] ,
				[[0,1], [1,1], [2,1], [3,1]] ,
				[[0,2], [1,2], [2,2], [3,2]] ,
				[[0,3], [1,3], [2,3], [3,3]] ,
			];
			break;
			
		case "down":
			lines = [
				[[3,3], [2,3], [1,3], [0,3]] ,
				[[3,2], [2,2], [1,2], [0,2]] ,
				[[3,1], [2,1], [1,1], [0,1]] ,
				[[3,0], [2,0], [1,0], [0,0]] ,
			];
			break;
	}

	var G = board.grid;
	var copies = [[],[],[],[]];
	for (var i=0; i < 4; i++) {
	for (var j=0; j < 4; j++) {
		var [y,x] = lines[i][j];
		copies[i][j] = G[y][x];
	} }

	var affectedLines = 0;
	var addedScore = 0;
	for (var i=0; i < 4; i++) {
		[affected, lineScore] = shiftMerge(copies[i]);
		affectedLines += affected;
		addedScore += lineScore;
	}

	if (updateMain) {
		for (var i=0; i < 4; i++) {
		for (var j=0; j < 4; j++) {
			var [y,x] = lines[i][j];
			G[i][j] = copies[y][x];
		} }
	}

	return [affectedLines, addedScore];
}

function shiftMerge(line) {
	var a = shiftLine(line);
	var b = mergeLine(line);
	var c = shiftLine(line);
	// affected, score
	return [a || c || b>0, b];	// TODO: check every case
}

function shiftLine(line) {
	var changed = false;
	var withoutZeros = [];
	for (var i=0; i < 4; i++) {
		if (line[i] != 0) {
			withoutZeros.push(line[i]);
	} }
	// shift without merging
	for (var i=0; i < 4; i++) {
		var newVal = withoutZeros[i] || 0;
		if (line[i] != newVal) { changed = true; }
		line[i] = newVal;
	}
	// return if shift did anything
	return changed;
}

function mergeLine(line) {
	var lineScore = 0;
	for (var i=0; i < 3; i++) {
		if (line[i] == line[i+1] != 0) {
			line[i]   = 2*line[i];
			line[i+1] = 0;
			lineScore += line[i];
	} }
	return lineScore;
}

function newNumber() {
	var rand = Math.random();
	return (rand < 0.5) ? 2 : 4;
}

function cellSet(pos, val) {
	[y,x] = pos;
	board.grid[y][x] = val;
}
function cellGet(pos) {
	[y,x] = pos;
	return board.grid[y][x];
}

function randomOpen() {
	var available = [];
	for (var y=0; y < 4; y++) {
	for (var x=0; x < 4; x++) {
		if (board.grid[y][x] == 0) {
			available.push([y,x]);
		}
	} }
	var index = Math.floor(Math.random() * available.length);
	return (available.length > 0) ? available[index] : null;
}




function draw() {
	var template =`
╔══════╤══════╤══════╤══════╗
║      │      │      │      ║
║00~~~~│01~~~~│02~~~~│03~~~~║
║      │      │      │      ║
╟──────┼──────┼──────┼──────╢
║      │      │      │      ║
║10~~~~│11~~~~│12~~~~│13~~~~║
║      │      │      │      ║
╟──────┼──────┼──────┼──────╢
║      │      │      │      ║
║20~~~~│21~~~~│22~~~~│23~~~~║
║      │      │      │      ║
╟──────┼──────┼──────┼──────╢
║      │      │      │      ║
║30~~~~│31~~~~│32~~~~│33~~~~║
║      │      │      │      ║
╚══════╧══════╧══════╧══════╝
`;
	var output = template;
	var style = "font-family:monospace; font-size: 20px";
	for (var y=0; y < 4; y++) {
	for (var x=0; x < 4; x++) {
		var number = board.grid[y][x];
		var formatted = number.toString();
		if (number == 0) { formatted = " "; }
		var padding = 6 - formatted.length;
		var padL = Math.round(padding/2);
		var padR = padding - padL;

		formatted = " ".repeat(padL) + formatted + " ".repeat(padR);
		output = output.replace(y.toString() + x.toString() + "~~~~", formatted);
	} }

	console.log("%c" + output, style);
}



setup();
input = "start"; run();
window.addEventListener("keydown", function(event) {
	if (getInput(event) != "exit") {
		run();
	} else {
		exit();
	}
});
