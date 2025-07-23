// ignore_for_file: deprecated_member_use

import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
  await Firebase.initializeApp();
  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'ESP32 IOT Control',
      home: const HomeScreen(),
    );
  }
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late DatabaseReference sensorRef;
  late DatabaseReference controlRef;

  late String latestKey = '';
  Map latest = {};
  Map<String, dynamic> control = {};

  @override
  void initState() {
    super.initState();
    sensorRef = FirebaseDatabase.instance.ref('latest');
    controlRef = FirebaseDatabase.instance.ref('control');
    _getLatestData();
    _getControlData();
  }

  void _getLatestData() {
    sensorRef.onValue.listen((event) {
      if (event.snapshot.exists) {
        final data = Map<String, dynamic>.from(event.snapshot.value as Map);
        setState(() {
          latest = data;
        });
      }
    });
  }

  void _getControlData() {
    controlRef.onValue.listen((event) {
      if (event.snapshot.exists) {
        final data = Map<String, dynamic>.from(event.snapshot.value as Map);
        setState(() {
          control = data;
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text(
          'ESP32 IOT Control',
          style: TextStyle(
            fontSize: 20,
            color: Colors.white,
            fontWeight: FontWeight.bold,
          ),
        ),
        backgroundColor: Colors.blueAccent,
        centerTitle: true,
      ),
      body: latest.isEmpty || control.isEmpty
          ? const Center(child: CircularProgressIndicator())
          : Padding(
              padding: const EdgeInsets.all(20.0),
              child: Column(
                children: [
                  CustomContainer(
                    title: 'Temperature',
                    image: 'assets/images/temp.png',
                    value: latest['temperature']?.toString() ?? 'N/A',
                  ),
                  const SizedBox(height: 20),
                  CustomContainer(
                    title: 'Humidity',
                    image: 'assets/images/humidity.png',
                    value: latest['humidity']?.toString() ?? 'N/A',
                  ),
                  const SizedBox(height: 20),
                  CustomContainer(
                    title: 'Led Brightness',
                    image: 'assets/images/brightness.png',
                    value:
                        "${(latest['led_brightness'] / 255 * 100).toStringAsFixed(2)}%",
                  ),
                  const SizedBox(height: 20),
                  LightContainer(
                    title: 'Light',
                    initialValue: control['led_state'] ?? 'off',
                    onToggle: (newState) {
                      controlRef.update({'led_state': newState});
                    },
                  ),
                  const SizedBox(height: 20),
                  LightContainer(
                    title: 'Buzzer',
                    initialValue: control['buzzer'] ?? 'off',
                    onToggle: (newState) {
                      controlRef.update({'buzzer': newState});
                    },
                  ),
                  const SizedBox(height: 20),
                  Container(
                    padding: const EdgeInsets.symmetric(
                      horizontal: 20,
                      vertical: 10,
                    ),
                    decoration: BoxDecoration(
                      color: Colors.blueAccent.shade400,
                      borderRadius: BorderRadius.circular(10),
                      boxShadow: [
                        BoxShadow(
                          color: Colors.grey.withOpacity(0.5),
                          spreadRadius: 2,
                          blurRadius: 5,
                          offset: const Offset(0, 3),
                        ),
                      ],
                    ),
                    child: Row(
                      children: [
                        const Text(
                          'Temperature Threshold',
                          style: TextStyle(
                            fontSize: 18,
                            color: Colors.white,
                            fontWeight: FontWeight.bold,
                          ),
                        ),
                        const Spacer(),
                        DropdownButton<int>(
                          iconEnabledColor: Colors.white,
                          iconDisabledColor: Colors.white,
                          dropdownColor: const Color.fromARGB(
                            255,
                            103,
                            124,
                            161,
                          ),
                          value: control['temp_threshold'],
                          items: List.generate(11, (index) => 25 + index)
                              .map(
                                (val) => DropdownMenuItem(
                                  value: val,
                                  child: Text(
                                    val.toString(),
                                    style: const TextStyle(color: Colors.white),
                                  ),
                                ),
                              )
                              .toList(),
                          onChanged: (value) {
                            if (value != null) {
                              controlRef.update({'temp_threshold': value});
                            }
                          },
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
    );
  }
}

class CustomContainer extends StatelessWidget {
  final String title, image;
  final String value;

  const CustomContainer({
    super.key,
    required this.title,
    required this.image,
    required this.value,
  });

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: Colors.blueAccent.shade400,
        borderRadius: BorderRadius.circular(10),
        boxShadow: [
          BoxShadow(
            color: Colors.grey.withOpacity(0.5),
            spreadRadius: 2,
            blurRadius: 5,
            offset: const Offset(0, 3),
          ),
        ],
      ),
      child: Row(
        children: [
          Image.asset(image, height: 40),
          const SizedBox(width: 10),
          Text(
            title,
            style: const TextStyle(
              fontSize: 18,
              color: Colors.white,
              fontWeight: FontWeight.bold,
            ),
          ),
          const Spacer(),
          Text(
            value,
            style: const TextStyle(
              fontSize: 18,
              color: Colors.white,
              fontWeight: FontWeight.bold,
            ),
          ),
        ],
      ),
    );
  }
}

class LightContainer extends StatefulWidget {
  final String initialValue;
  final Function(String) onToggle;
  final String title;

  const LightContainer({
    super.key,
    required this.initialValue,
    required this.onToggle,
    required this.title,
  });

  @override
  State<LightContainer> createState() => _LightContainerState();
}

class _LightContainerState extends State<LightContainer> {
  late bool isOn;

  @override
  void initState() {
    super.initState();
    isOn = widget.initialValue == 'on';
  }

  void _toggle(bool value) {
    setState(() {
      isOn = value;
    });
    widget.onToggle(isOn ? 'on' : 'off');
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(20),
      decoration: BoxDecoration(
        color: Colors.blueAccent.shade400,
        borderRadius: BorderRadius.circular(10),
        boxShadow: [
          BoxShadow(
            color: Colors.grey.withOpacity(0.5),
            spreadRadius: 2,
            blurRadius: 5,
            offset: const Offset(0, 3),
          ),
        ],
      ),
      child: Row(
        children: [
          Image.asset(
            widget.title == 'Light'
                ? 'assets/images/lamp.png'
                : 'assets/images/alarm.png',
            height: 40,
          ),
          const SizedBox(width: 10),
          Text(
            widget.title,
            style: const TextStyle(
              fontSize: 18,
              color: Colors.white,
              fontWeight: FontWeight.bold,
            ),
          ),
          const Spacer(),
          Switch(
            value: isOn,
            onChanged: _toggle,
            activeColor: Colors.white,
            activeTrackColor: Colors.greenAccent.shade400,
          ),
        ],
      ),
    );
  }
}
