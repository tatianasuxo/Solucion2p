import React from 'react';
import "./MilkshakeMain.css"
import MilkshakeCard from './MilkshakeCard';
import { MilkshakeContainer } from './MilkshakeContainer';
export const MilkshakeMain = () => {
  return (
    <div className='main-container'>
        <h2>Milk Shakes</h2> 
        <div className='cards-container'>
          <MilkshakeContainer/>
        </div>
    </div>
  )
}


