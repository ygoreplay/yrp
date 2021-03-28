// eslint-disable-next-line @typescript-eslint/no-var-requires
const addon = require("bindings")("yrp.node");

export function uncompressReplayData(buffer: Buffer): Buffer {
    return addon.uncompressReplayData(buffer);
}
