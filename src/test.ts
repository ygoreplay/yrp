import * as fs from "fs";
import * as path from "path";

import { Replay } from ".";

(async () => {
    const buffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
    const replay = await Replay.fromBuffer(buffer);

    console.log(replay.getReplayHeader());
    console.log(replay.getPlayerNames());
    console.info(replay.getReplayScriptName());
    console.info(replay.getReplayParameter());
})();
