import { Replay } from ".";

import * as fs from "fs";
import path from "path";

describe("yrp", () => {
    it("should retrieve replay header information", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = await Replay.fromBuffer(originalFileBuffer);

        expect(replay.getReplayHeader()).toMatchSnapshot();
    });

    it("should retrieve replay header information from file", async () => {
        const replay = await Replay.fromFile(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        expect(replay.getReplayHeader()).toMatchSnapshot();
    });

    it("should retrieve player names", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = await Replay.fromBuffer(originalFileBuffer);

        expect(replay.getPlayerNames()).toMatchSnapshot();
    });
});
