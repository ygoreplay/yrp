// @ts-ignore
import * as fs from "fs";

const addon = require("bindings")("yrp-replay-addon.node");
const buffer = fs.readFileSync("../replay-parser/res/yrp-basic.yrp");

console.log(addon.uncompressReplayData(buffer));
