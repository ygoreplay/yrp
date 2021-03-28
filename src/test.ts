import * as fs from "fs";
import * as path from "path";

// eslint-disable-next-line @typescript-eslint/no-var-requires
const addon = require("bindings")("yrp.node");
const buffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));

console.log(addon.uncompressReplayData(buffer));
