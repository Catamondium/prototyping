#!/usr/bin/env node

const COEFFRE = /^(\d+)/g // beginning coefficient
const TOKRE = /\(.*?\)|([A-Z][a-z]*)(\d*)/g  // groups: Elem, [coeff]
const SUBRE = /\((.*)\)(\d*)/g // groups: expr, [coeff]

function makeCoeff(c) {
    if (c == '' || c === undefined) {
        return 1;
    } else {
        return parseInt(c);
    }
}

function sanitize(thing) {
    thing = thing.replace(/[\[\{\}\]]/g, match => {
        dict = {
            '[': '(',
            ']': ')',
            '{': '(',
            '}': ')'
        }
        return dict[match];
    });
    arr = thing.split('');
    arr = arr.filter(c => c.match(/[a-z0-9\(\)]/i));
    return arr.join('');
}

ptable = {
    H: 1,
    C: 12,
    undefined: 0 // undefined is keyable?
}

// if(__name__ == '__main__') eqiv, supporting web embedding
if (typeof require != 'undefined' && require.main == module) {
    test = "CH3(CH3)2CH3"
    while ((m = TOKRE.exec(test)) !== null) {
        groups = m.slice(1);
        console.log(`E:\t${ptable[groups[0]]}\tCoeff:\t${makeCoeff(groups[1])}`);
    }

    while ((m = SUBRE.exec(test)) !== null) {
        groups = m.slice(1);
        console.log(`Exp:\t${ptable[groups[0]]} Coeff:\t${makeCoeff(groups[1])}`);
    }

    console.log(sanitize("C224[](){}<>?££$£"))
}