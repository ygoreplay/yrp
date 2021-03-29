import { Replay } from ".";

import * as fs from "fs";
import path from "path";

describe("yrp", () => {
    it("should retrieve replay header information", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = Replay.fromBuffer(originalFileBuffer);

        expect(replay.getHeader()).toMatchSnapshot();
    });

    it("should retrieve replay header information from file", async () => {
        const replay = await Replay.fromFile(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        expect(replay.getHeader()).toMatchSnapshot();
    });

    it("should retrieve player names", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = Replay.fromBuffer(originalFileBuffer);

        expect(replay.getPlayerNames()).toMatchSnapshot();
    });

    it("should retrieve replay parameters", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = Replay.fromBuffer(originalFileBuffer);

        expect(replay.getParameter()).toMatchSnapshot();
    });

    it("should retrieve replay script name", async () => {
        const originalFileBuffer = fs.readFileSync(path.join(process.cwd(), "./res/yrp-basic.yrp"));
        const replay = Replay.fromBuffer(originalFileBuffer);

        expect(replay.getScriptName()).toMatchSnapshot();
    });
});
