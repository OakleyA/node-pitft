var bindings = require('bindings')('pitftnapi');

function pitft(arg1, arg2) {
    return new bindings.FrameBuffer(process.cwd(), arg1, arg2);
}

module.exports = pitft;
