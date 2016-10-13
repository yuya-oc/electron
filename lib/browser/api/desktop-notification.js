const {EventEmitter} = require('events')
const {create, DesktopNotification} = process.atomBinding('desktop_notification')

Object.setPrototypeOf(DesktopNotification.prototype, EventEmitter.prototype)

module.exports = {create}
