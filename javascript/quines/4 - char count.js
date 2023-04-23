let charCount = [48, 0, 4, 2, 0, 0, 0, 0, 14, 14, 0, 7, 5, 0, 13, 0, 1, 0, 2, 2, 0, 1, 1, 0, 0, 1, 0, 10, 1, 13, 2, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0, 0, 13, 1, 18, 4, 23, 6, 6, 11, 12, 0, 3, 16, 4, 11, 23, 11, 0, 28, 8, 32, 18, 0, 0, 0, 1, 0, 1, 0, 1, 0];

let repl = String.fromCharCode(35);
let tick = String.fromCharCode(96);
let output = `let charCount = [#];

let repl = String.fromCharCode(35);
let tick = String.fromCharCode(96);
let output = #;
for (let i=0; i<charCount.length; i++) {
	char = String.fromCharCode(i+32);
	console.assert(charCount[i] == Array.from(output).filter((c) => c == char).length);
}
output = output.replace(repl, charCount.reduce((s,c) => s+", "+c, "").substr(2))
               .replace(repl, tick+output+tick);
console.log(output);`;
for (let i=0; i<charCount.length; i++) {
	char = String.fromCharCode(i+32);
	console.assert(charCount[i] == Array.from(output).filter((c) => c == char).length);
}
output = output.replace(repl, charCount.reduce((s,c) => s+", "+c, "").substr(2))
               .replace(repl, tick+output+tick);
console.log(output);