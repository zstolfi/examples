let E = (n) => String.fromCharCode(n);
let a = [
"let E = (n) => String.fromCharCode(n);",
"let a = [",
"];",
"let output = '';",
"for (let i=0; i < a.length; i++) {",
"    if (i == 2) { output += a.reduce((s,c) => s+E(34)+c+E(34)+','+E(10), ''); }",
"    output += a[i]+E(10);",
"}",
"console.log(output);",
];
let output = '';
for (let i=0; i < a.length; i++) {
    if (i == 2) { output += a.reduce((s,c) => s+E(34)+c+E(34)+','+E(10), ''); }
    output += a[i]+E(10);
}
console.log(output);