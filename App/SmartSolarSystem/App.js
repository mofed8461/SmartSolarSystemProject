import React, { Component } from 'react';
import { StyleSheet, Text, View, TextInput, Button, TouchableOpacity, AsyncStorage, Alert } from 'react-native';
import { getAppLoadingLifecycleEmitter } from 'expo/build/launch/AppLoading';

export default function App() {
  return (<Screens />);
}

class Screens extends Component {
  state = {
    currentScreen: 'Home',
    connected: false,
    timerID: null,
    voltage: 0,
    wattsPerMin: [],
    wattsPerHour: [],
    wattsPerDay: [],
    wattsPerWeek: []
  }

  async componentDidMount()
  {
    this.counter = 0;

    try 
    {
      if (await AsyncStorage.getItem('firstTime') == 'true')
      {
        this.setState(JSON.parse(await AsyncStorage.getItem('state')));
      }
      else
      {
        for (var i = 0; i < 6; i++)
          this.state.wattsPerMin.push(0);
        
        for (var i = 0; i < 6 * 60; i++)
          this.state.wattsPerHour.push(0);

        for (var i = 0; i < 6 * 60 * 24; i++)
          this.state.wattsPerDay.push(0);

        for (var i = 0; i < 6 * 6 * 24 * 7; i++)
          this.state.wattsPerWeek.push(0);
        
        await AsyncStorage.setItem('firstTime', 'true');
        await AsyncStorage.setItem('state', JSON.stringify(this.state));
      }
    }
    catch(error)
    {
      console.log(JSON.stringify(error));
    }
  }

  getAvgMin()
  {
    var avg = 0;
    for (var i = 0; i < 6; i++)
      avg += this.state.wattsPerMin[i];

    return avg / 6.0;
  }

  getAvgHour()
  {
    var avg = 0;
    for (var i = 0; i < 6 * 60; i++)
      avg += this.state.wattsPerHour[i];
      
    return avg / (6.0 * 60.0);
  }

  getAvgMDay()
  {
    var avg = 0;
    for (var i = 0; i < 6 * 6 * 24; i++)
      avg += this.state.wattsPerDay[i * 10];
      
    return avg / (6 * 6 * 24.0);
  }

  getAvgMWeek()
  {
    var avg = 0;
    for (var i = 0; i < 6 * 6 * 24 * 7; i++)
      avg += this.state.wattsPerWeek[i];
      
    return avg / (6 * 6 * 24 * 7.0);
  }

  getAvgMonth()
  {
    return this.getAvgMWeek() * 0.99 + this.getAvgMDay() * 0.01;
  }

  ffetch(url)
  {
    console.log('requesting: ' + url);
    return fetch(url);
    // return fetch(url, {
      // method: 'GET',
      // headers: {
      //   // Accept: 'application/json',
      //   // 'Content-Type': 'application/json',
      // },
      // body: JSON.stringify({}),
    // });
  }

  startTimer()
  {
    var timerID = setInterval(() => {
      console.log('loop');
      this.ffetch('http://192.168.4.1/c')
      .then((response) => response.text())
      .then((responseJson) => {
        console.log(JSON.stringify(responseJson));
        var success = (responseJson == '1234');
        this.setState({connected: success});
        return success;
      })
      .then((success) =>  {
        if (success)
        {
          console.log('fetching voltage');

          return this.ffetch('http://192.168.4.1/v');
        }
        else
        {
          throw 'failed to connect';
        }
      })
      .then((response) => response.text())
      .then(async (voltage) => {
        console.log(JSON.stringify(voltage));
        
        await this.logic(voltage);
      }).catch((error) => {

        console.log('error: ' + JSON.stringify(error));

        this.setState({connected: false});
      });

    }, 10000);

    this.setState({timerID: timerID});
  }

  stopTimer()
  {
    clearInterval(this.state.timerID);
    this.setState({timerID: null});
  }

  StartStopTimer()
  {
    if (this.state.timerID == null) 
      this.startTimer(); 
    else 
      this.stopTimer();
  }

  render()
  {
    if (this.state.currentScreen == 'Home')
    {

      var StartStopTimer = null;
      {
        StartStopTimer = (
          <View style={styles.hor}>
            <TouchableOpacity style={[styles.Input, {width: '70%', alignSelf: 'center'}]} onPress={() => this.StartStopTimer()}>
              <Text>{this.state.timerID == null ? 'System is off' : 'System is on'}</Text>
            </TouchableOpacity>
          </View>
        );
      }

      var status = [(
        <Text key={'status'} style={styles.sText}>Status: {this.state.connected? 'connected' : 'not connected'}</Text>
      )];

      {
        var otherStyle = null;
        if (!this.state.connected)
        {
          otherStyle = styles.hidden;
        }

        status.push(
          <Text key={'voltage'} style={[styles.sText, otherStyle]}>Voltage: {this.state.voltage}</Text>
        );

        status.push(
          <Text key={'wph'} style={[styles.sText, otherStyle]}>Watts / Hour: {this.getAvgHour()}</Text>
        );

        status.push(
          <Text key={'wpm'} style={[styles.sText, otherStyle]}>Average Watts in last minute: {this.getAvgMin()}</Text>
        );

        status.push(
          <Text key={'wpd'} style={[styles.sText, otherStyle]}>Average Watts in last Day: {this.getAvgMDay()}</Text>
        );
       
        status.push(
          <Text key={'wpw'} style={[styles.sText, otherStyle]}>Average Watts in last Week: {this.getAvgMWeek()}</Text>
        );

        status.push(
          <Text key={'wpMm'} style={[styles.sText, otherStyle]}>Average Watts in last Month: {this.getAvgMonth()}</Text>
        );
       
      }



      return (
      <View style={styles.container}>
        {status}
        {StartStopTimer}
      </View>
      );
    }
    return null;
  }



  async logic(voltage)
  {
    console.log('ll1');
    var watts = voltage * 0.166;

    var stateCopy = this.state;

    stateCopy.voltage = voltage;

    if (voltage < 0.1)
    {
      Alert.alert('', 'Not connected');
    }
    else if (voltage < 0.5)
    {
      Alert.alert('', 'Low voltage');
    }


    stateCopy.wattsPerMin[this.counter % stateCopy.wattsPerMin.length] = watts;
    stateCopy.wattsPerHour[this.counter % stateCopy.wattsPerHour.length] = watts;
    stateCopy.wattsPerDay[this.counter % stateCopy.wattsPerDay.length] = watts;
    stateCopy.wattsPerWeek[this.counter % stateCopy.wattsPerWeek.length] = watts;
    console.log('ll2');

    this.setState(stateCopy);
    console.log('ll3');
    this.counter++;

    await AsyncStorage.setItem('state', JSON.stringify(this.state));
    console.log('ll4');
  }
}


const styles = StyleSheet.create({
  container: {
    flex: 1,
    marginTop: 60,
    backgroundColor: '#fff',
    alignItems: 'center',
    // justifyContent: 'center',
  },
  hor:{
    flexDirection: 'row'
  },
  Input:{
    alignItems: 'center',
    width: 60,
    borderWidth: 1,
    borderRadius: 10,
    margin: 10,
    padding: 10
  },
  Text:{
    alignItems: 'center',
    borderWidth: 0,
    borderRadius: 10,
    margin: 10,
    padding: 10
  },
  sText:{
    alignItems: 'center',
    borderWidth: 0,
    borderRadius: 10,
    margin: 2,
    padding: 2
  },
  hidden: {
    opacity: 0.3,
  }
  
});