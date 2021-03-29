import * as fs from "fs";
import * as path from "path";

import { Replay } from ".";

(async () => {
    const buffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
    const replay = await Replay.fromBuffer(buffer);

    console.log(replay.getHeader());
    console.log(replay.getPlayerNames());
    console.info(replay.getScriptName());
    console.info(replay.getParameter());
})();
