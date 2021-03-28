import * as fs from "fs";
import * as path from "path";

import { Replay } from ".";

import * as SegfaultHandler from "segfault-handler";

SegfaultHandler.registerHandler("crash.log");

(async () => {
    const buffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
    const replay = await Replay.fromBuffer(buffer);

    console.log(replay.getReplayHeader().props.length);
})();
