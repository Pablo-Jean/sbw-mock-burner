/*
 * hECDSA.h
 *
 *  Created on: 30 de set de 2021
 *      Author: pablo.jean
 */

#ifndef CC_HDRIVER_HECDSA_H_
#define CC_HDRIVER_HECDSA_H_


#include <stdint.h>


/*
 * Macros
 */

/*
 * Enumerates
 */

typedef enum{
    hECDSA_OK,
    hECDSA_FAIL,
    hECDSA_INVALID,
    hECDSA_NULL = 0xFF
}hEcdsa_error_e;

typedef enum{
    hECDSA_NISTP224,
    hECDSA_NISTP256,
    hECDSA_NISTP384,
    hECDSA_NISTP521,
    hECDSA_BrainpoolP256R1,
    hECDSA_BrainpoolP384R1,
    hECDSA_BrainpoolP512R1
}hEcdsa_curve_e;


/*
 * Function Prototypes
 */

hEcdsa_error_e hECDSA_verify(hEcdsa_curve_e curve, uint8_t *hash, uint8_t *r, uint8_t *s, uint8_t *publicKey, uint16_t keySize);


#endif /* CC_HDRIVER_HAES_H_ */
