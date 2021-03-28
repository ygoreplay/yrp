<h1 align="center">
  <br />
  <img src="https://raw.githubusercontent.com/ygoreplay/yrp/master/res/logo.png" />
  <br />
  yrp
  <sup>
    <br />
    <br />
  </sup>
</h1>

<div align="center">
    <img alt="GitHub Workflow Status" src="https://img.shields.io/github/workflow/status/ygoreplay/yrp/CI?style=flat-square">
    <img alt="npm" src="https://img.shields.io/npm/v/yrp?style=flat-square">
    <img alt="NPM" src="https://img.shields.io/npm/l/yrp?style=flat-square">
    <img alt="Codecov" src="https://img.shields.io/codecov/c/github/ygoreplay/yrp?style=flat-square">
  <br />
  <sup>Just a parser for YRP1 or YRPX format but on super-fast performance!</sup>
  <br />
  <br />
</div>

## Purpose of development

We already know most of the YGOPro/EDOPro related projects are quite obsolete and hard to contribute since they are developed in quite low level (or not-using) languages. (e.g. CoffeeScript, C/C++, etc.) we know those ecosystems will eventually occur tons of problem. so I had decided to resolve this issues in _**modern**_ and _**elegant**_ way.

so this is a native parser project for node environment. this means I want to live lazy like other webdevs 😂. but you would  realize that providing modules for productive languages  can have a lot of impact on improving development ecosystem of YGOPro/EDOPro.

## Installation

*Note: you should use Node with version >= 12.*

```
npm i yrp
```

or if you use [yarn](https://github.com/yarnpkg/berry):

```
yarn add yrp
```

## How to use

```tsx
import { Replay } from "yrp";

// ...

// of course you can load replay from raw buffer too!
const replay = await Replay.fromFile("./target-file.yrp");
```
