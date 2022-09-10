import React, {useEffect, useState} from 'react';
import {Link} from 'react-router-dom';
import ConfigForm from "./ConfigForm";
import axios from 'axios';
import './App.css';

const defaultConfig = {
    broker: '',
    token: '',
    port: '',
    lat: '',
    long: '',
    serial: '',
    model: '',
    tags: '',
    calibration: '',
    calibration2: '',
    calibrationAna4: '',
    calibrationAna5: '',
    useRfid: true,
    flow1: false,
    flow2: false,
}

const App = () => {
    const [ip, setIp] = useState('');
    const [configs, setConfigs] = useState(defaultConfig);
    useEffect(() => {
        const f = async () => {
            const {data} = await axios.get('/ip');
            if (data.localIp) {
                setIp(data.localIp);
            }
        }
        f();

        const f2 = async () => {
            const {data} = await axios.get('/config/config');
            setConfigs({
                ...data,
                useRfid: data.useRfid === 'true',
            });
        }
        f2();
    }, []);

    const save = async () => {
        if(isNaN(Number(configs.lat)) || isNaN(Number(configs.long))) {
            alert('Formato de latitude ou longitude inválido! Utilize apenas números.');
            return;
        }
        const json = JSON.stringify(configs);
        const res = await axios.post('/config/config', json, {
            headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
            }
        });
        alert('Configurações salvas com sucesso!');
    }
    return (
        <div>
            <div class="header">
            <Link to='/' class="link" style={{fontWeight: 'bold'}}>
                    ESP32 Controle de fluxo
                </Link>
                <div>
                     <Link to='/' class="link">
                       Home
                    </Link>
                    <Link to='/logout' class="link">
                       Sair
                    </Link>
                </div>
            </div>
            <div style={{display: "flex", justifyContent: "center", marginTop: "10px", marginBottom: "10px"}}>
              <div class="body">
                  <div>
                      Para acesso via celular, acesse:
                  </div>
                  <div>
                      <input disabled value={ip} style={{marginTop: "5px", padding: "5px 20px", textAlign: 'center'}}></input>
                  </div>
                  <div style={{ marginTop: '10px', marginBottom: '10px', width: '70vw', color: '#0000aa', borderTopStyle: 'solid', borderWidth: '2px'}}/>
                  <div style={{maxHeight: '60vh', overflowY: 'scroll'}}>
                    <ConfigForm configs={configs} setConfigs={setConfigs}/>
                  </div>
                  <div style={{ marginTop: '10px', marginBottom: '10px', width: '70vw', color: '#0000aa', borderTopStyle: 'solid', borderWidth: '2px'}}/>
                  <div>
                      <button class="button button3" onClick={save}>Salvar</button>
                  </div>
              </div>
            </div>
            <div class="footer">
            </div>
        </div>
    );
};

export default App;