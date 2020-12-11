// start.js

Page({
    data: {

    },
    //跳转到温度页面
    navigate: function() {
        wx.navigateTo({
            url: '../wifi_station/tianqi/tianqi',
        })
    },
    //跳转到湿度页面
    navigate1: function() {
        wx.navigateTo({
            url: '../wifi_station/a',
        })
    },
    //跳转到knowledge页面
    navigate2: function() {
        wx.navigateTo({
            url: '../wifi_station/tianqi/knowledge',
        })
    }
})