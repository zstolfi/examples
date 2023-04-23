let textfield = document.createElement("textarea");
let outText = document.createElement("pre");
document.body.appendChild(textfield);
document.body.appendChild(outText);

function calculate() {
    let input = textfield.value;
    let output = "";
    for (let i=32; i<=126; i++) {
        let c = String.fromCharCode(i);
        output += c + "    : ";
        output += "#".repeat(Array.from(input).filter((d) => c==d).length);
        output += "\n";
    }
    outText.innerText = output;
}
textfield.style = "position: fixed; right: 8px;"
                + "width: 400px; height: 300px;";
textfield.setAttribute("oninput","calculate();");

calculate();