import React from 'react';
import './style.css';

const ConfigForm = ({configs, setConfigs}) => {
    const changeField = evt => {
        const value = evt.target.value;
        const field = evt.target.name;
        setConfigs(old => ({...old, [field]: value}));
    };

    return (
        <div >
            <div>
                Broker server:
                <input onChange={changeField} name="broker" value={configs.broker} style={{marginLeft: '4px', width: '200px'}}/>
            </div>
            <div>
                Broker server Porta:
                <input onChange={changeField} name="port" value={configs.port} style={{marginLeft: '4px', width: '150px'}}/>
            </div>
            <div>
                Device Token:
                 <input onChange={changeField} name="token" value={configs.token} style={{marginLeft: '4px', width: '200px'}}/>
            </div>
            <div style={{marginTop: '15px'}}>
                Lista de TAGs URL:
                <input onChange={changeField} name="tags" value={configs.tags} style={{marginLeft: '4px', width: '260px'}}/>
            </div>
            <div style={{marginTop: '15px'}}>
                Latitude:
                <input onChange={changeField} name="lat" value={configs.lat} style={{marginLeft: '18px', width: '100px'}}/>
            </div>
            <div>
                Longitude:
                <input onChange={changeField} name="long" value={configs.long} style={{marginLeft: '4px', width: '100px'}}/>
            </div>
            <div style={{marginTop: '15px'}}>
                Serial do produto:
                <input onChange={changeField} name="serial" value={configs.serial} style={{marginLeft: '20px', width: '250px'}}/>
            </div>
            <div>
                Modelo do produto:
                <input onChange={changeField} name="model" value={configs.model} style={{marginLeft: '4px', width: '250px'}}/>
            </div>
            <div style={{marginTop: '15px'}}>
                Fator de calibração fluxo 1:
                <input onChange={changeField} name="calibration" value={configs.calibration} style={{marginLeft: '4px', width: '200px'}}/>
            </div>
            <div>
                Fator de calibração fluxo 2:
                <input onChange={changeField} name="calibration2" value={configs.calibration2} style={{marginLeft: '4px', width: '200px'}}/>
            </div>
            <div>
                Fator de calibração ANA 4:
                <input onChange={changeField} name="calibrationAna4" value={configs.calibrationAna4} style={{marginLeft: '4px', width: '200px'}}/>
            </div>
            <div>
                Fator de calibração ANA 5:
                <input onChange={changeField} name="calibrationAna5" value={configs.calibrationAna5} style={{marginLeft: '4px', width: '200px'}}/>
            </div>
            <div style={{marginTop: '15px'}}>
                Utilizar o RFID:
                <input type="checkbox" onClick={() => setConfigs(old => ({...old, 'useRfid': !configs.useRfid}))} name="useRfid" checked={configs.useRfid}/>
            </div>
            <div style={{marginTop: '15px'}}>
                Sensor de fluxo 1:
                <input type="checkbox" onClick={() => setConfigs(old => ({...old, 'flow1': !configs.flow1}))} name="flow1" checked={configs.flow1}/>
            </div>
            <div>
                Sensor de fluxo 2:
                <input type="checkbox" onClick={() => setConfigs(old => ({...old, 'flow2': !configs.flow2}))} name="flow2" checked={configs.flow2}/>
            </div>
        </div>
    );
};

export default ConfigForm;