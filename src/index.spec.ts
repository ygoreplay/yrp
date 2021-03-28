import { Replay } from ".";

import * as fs from "fs";
import path from "path";

describe("yrp", () => {
    it("should uncompress replay header information properly", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = await Replay.fromBuffer(originalFileBuffer);

        expect(replay.getReplayHeader()).toMatchSnapshot();
    });

    it("should uncompress replay header information from file properly", async () => {
        const replay = await Replay.fromFile(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        expect(replay.getReplayHeader()).toMatchSnapshot();
    });
});
