import React, {useEffect, useState} from 'react';
import {Link} from 'react-router-dom';
import axios from 'axios';
import './App.css';

const App = () => {
    const [ip, setIp] = useState('');
    const [flow1, setFlow1] = useState('');
    const [flow2, setFlow2] = useState('');
    const [inputs, setInputs] = useState('');
    const [ana_in_4, setAna_in_4] = useState('');
    const [ana_in_5, setAna_in_5] = useState('');
    useEffect(() => {
        const f = async () => {
            const {data} = await axios.get('/ip');
            if (data.localIp) {
                setIp(data.localIp);
            }
        }
        f();

        setInterval(async () => {
            const {data} = await axios.get('/flow');
            if (data.flow1 || data.flow2) {
                setFlow1(data.flow1);
                setFlow2(data.flow2);
            }

            const {data: data2} = await axios.get('/inputs');
            if (data2) {
                setInputs(data2.input);
                setAna_in_4(data2.ANA_IN_4);
                setAna_in_5(data2.ANA_IN_5);
            }
        }, 3000);
    }, []);

    
    const save = async () => {
    }
    return (
        <div>
            <div class="header">
                <Link to='/' class="link" style={{fontWeight: 'bold'}}>
                    ESP32 Controle de fluxo
                </Link>
                <div>
                     <Link to='/config' class="link">
                       Configurações
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
                  <div>
                      Fluxo 1 instantâneo:
                      <input disabled value={flow1} style={{marginTop: "5px", padding: "5px 20px", marginLeft: "5px", width: "100px"}}></input>
                      L/min
                  </div>
                  <div>
                      Fluxo 2 instantâneo:
                      <input disabled value={flow2} style={{marginTop: "5px", padding: "5px 20px", marginLeft: "5px", width: "100px"}}></input>
                      L/min
                  </div>
                  <div style={{marginTop: '12px'}}>
                      {
                          [0,1,2,3,4,5,6,7].map(cur => 
                            <div>
                                DIGI_IN_{cur}:
                                    <input disabled value={(inputs >> cur) % 2} style={{marginTop: "5px", padding: "5px 20px", marginLeft: "5px", width: "20px"}}></input>
                            </div>
                        )
                      }
                  </div>
                  <div style={{marginTop: '12px'}}>
                     ANA_IN_4:
                        <input disabled value={(ana_in_4 * 3.3 / 4096).toFixed(2)} style={{marginTop: "5px", padding: "5px 20px", marginLeft: "5px", width: "50px"}}></input>V
                  </div>
                  <div>
                    ANA_IN_5:
                        <input disabled value={(ana_in_5 * 3.3 / 4096).toFixed(2)} style={{marginTop: "5px", padding: "5px 20px", marginLeft: "5px", width: "50px"}}></input>V
                  </div>
                </div>
            </div>
            <div class="footer">
            </div>
        </div>
    );
};

export default App;